
#include <Eigen/SVD>
#include <Eigen/Geometry>

template < class DataPoint, class _WFunctor, typename T>
void
MongePatch<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_x.setZero();
    m_planeIsReady = false;
}

template < class DataPoint, class _WFunctor, typename T>
bool
MongePatch<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    if(! m_planeIsReady)
    {
        return Base::addNeighbor(_nei);
    }
    else // base plane is ready, we can now fit the patch
    {
        VectorType q = _nei.pos() - Base::basisCenter();
        Scalar w = Base::m_w.w(q, _nei);

        if (w > Scalar(0.))
        {
            // express neighbor in local coordinate frame
            VectorType local = Base::worldToTangentPlane(_nei.pos());
            const Scalar& h = *(local.data());
            const Scalar& u = *(local.data()+1);
            const Scalar& v = *(local.data()+2);

            Eigen::Matrix<Scalar, 6, 1 > p;
            p << u*u, v*v, u*v, u, v, 1;
            m_A    += w*p*p.transpose();
            m_b    += w*h*p;

            return true;
        }
    }
    return false;
}

template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
MongePatch<DataPoint, _WFunctor, T>::finalize ()
{
    // end of the fitting process, check plane is ready
    if (! m_planeIsReady) {
        FIT_RESULT res = Base::finalize();

        if(res == STABLE) {  // plane is ready
            m_planeIsReady = true;
            m_A = MatrixX(6,6/*5*/);
            m_A.setZero();
            m_b = VectorX(6);
            m_b.setZero();
            m_neiIdx = 0;

            return Base::m_eCurrentState = NEED_OTHER_PASS;
        }
        return res;
    }
    // end of the monge patch fitting process
    else {
        // we use BDCSVD as the matrix size is 36
        // http://eigen.tuxfamily.org/dox/classEigen_1_1BDCSVD.html
        m_x = m_A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(m_b);
        return Base::m_eCurrentState = STABLE;
    }
}

template < class DataPoint, class _WFunctor, typename T>
typename MongePatch<DataPoint, _WFunctor, T>::Scalar
MongePatch<DataPoint, _WFunctor, T>::kMean() const {
  PONCA_MULTIARCH_STD_MATH(pow);
  static const Scalar one (1);
  static const Scalar two (2);
  static const Scalar threeOverTwo (Scalar(3)/Scalar(2));
  return ((one + pow(h_v(),two) ) * h_uu() * two*h_u()*h_v()*h_uv() + (one+pow(h_u(),two))*h_vv()) /
      (two * pow(one +pow(h_u(),two) + pow(h_v(),two),threeOverTwo));
}

template < class DataPoint, class _WFunctor, typename T>
typename MongePatch<DataPoint, _WFunctor, T>::Scalar
MongePatch<DataPoint, _WFunctor, T>::GaussianCurvature() const {
    PONCA_MULTIARCH_STD_MATH(pow);
    static const Scalar one (1);
    static const Scalar two (2);
    return (h_uu()*h_vv() - pow(h_uv(),two)) /
        pow((one + pow(h_u(),two) + pow(h_v(),two) ), two);
}
