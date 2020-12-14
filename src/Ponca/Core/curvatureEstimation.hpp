
template < class DataPoint, class _WFunctor, typename T>
void
BaseCurvatureEstimator<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_k1 = 0;
    m_k2 = 0;
    m_v1 = VectorType::Zero();
    m_v2 = VectorType::Zero();
}


template < class DataPoint, class _WFunctor, typename T>
void
NormalCovarianceCurvature<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_cov = MatrixType::Zero();
    m_cog = VectorType::Zero();
}

template < class DataPoint, class _WFunctor, typename T>
bool
NormalCovarianceCurvature<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    bool bResult = Base::addNeighbor(_nei);

    if(bResult)
    {
        m_cov += _nei.normal() * _nei.normal().transpose();
        m_cog += _nei.normal();
    }
    return bResult;
}

template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
NormalCovarianceCurvature<DataPoint, _WFunctor, T>::finalize ()
{
    typedef typename VectorType::Index Index;

    PONCA_MULTIARCH_STD_MATH(abs);

    FIT_RESULT res = Base::finalize();

    if(this->isReady())
    {
        // center of gravity (mean)
        m_cog /= Base::m_nbNeighbors;

        // covariance matrix
        m_cov /= Base::m_nbNeighbors;
        m_cov -= m_cog*m_cog.transpose();

        m_solver.computeDirect(m_cov);

        Base::m_k1 = m_solver.eigenvalues()(1);
        Base::m_k2 = m_solver.eigenvalues()(2);

        Base::m_v1 = m_solver.eigenvectors().col(1);
        Base::m_v2 = m_solver.eigenvectors().col(2);

        //TODO(thib) which epsilon value should be chosen ?
//        Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();
        Scalar epsilon = Scalar(1e-3);
        if(Base::m_k1<epsilon && Base::m_k2<epsilon)
        {
            Base::m_k1 = Scalar(0);
            Base::m_k2 = Scalar(0);

            // set principal directions from normals center of gravity
            VectorType n = m_cog.normalized();
            Index i0 = -1, i1 = -1, i2 = -1;
            n.rowwise().squaredNorm().minCoeff(&i0);
            i1 = (i0+1)%3;
            i2 = (i0+2)%3;
            Base::m_v1[i0] = 0;
            Base::m_v1[i1] = n[i2];
            Base::m_v1[i2] = -n[i1];
            Base::m_v2[i0] = n[i1]*n[i1] + n[i2]*n[i2];
            Base::m_v2[i1] = -n[i1]*n[i0];
            Base::m_v2[i2] = -n[i2]*n[i0];
        }
    }
    return res;
}

template < class DataPoint, class _WFunctor, typename T>
void
ProjectedNormalCovarianceCurvature<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_cog = Vector2::Zero();
    m_cov = Mat22::Zero();
    m_pass = FIRST_PASS;
    m_tframe = Mat32::Zero();
}

template < class DataPoint, class _WFunctor, typename T>
bool
ProjectedNormalCovarianceCurvature<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    if(m_pass == FIRST_PASS)
    {
        return Base::addNeighbor(_nei);
    }
    else if(m_pass == SECOND_PASS)
    {
        VectorType q = _nei.pos() - Base::m_evalPos;
        if(Base::m_w.w(q, _nei)>0)
        {
            // project normal on plane
            VectorType n = _nei.normal();
            Vector2 proj = m_tframe.transpose() * n;

            m_cov += proj * proj.transpose();
            m_cog += proj;
            return true;
        }
    }
    return false;
}

template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
ProjectedNormalCovarianceCurvature<DataPoint, _WFunctor, T>::finalize ()
{
    if(m_pass == FIRST_PASS)
    {
        FIT_RESULT res = Base::finalize();

        if(res != UNDEFINED)
        {
            // get normal of fitted plane
            VectorType n = this->primitiveGradient(VectorType());

            // compute orthonormal frame of the tangent plane
            Index i0 = -1, i1 = -1, i2 = -1;
            n.rowwise().squaredNorm().minCoeff(&i0);
            i1 = (i0+1)%3;
            i2 = (i0+2)%3;
            m_tframe.col(0)[i0] = 0;
            m_tframe.col(0)[i1] = n[i2];
            m_tframe.col(0)[i2] = -n[i1];
            m_tframe.col(1)[i0] = n[i1]*n[i1] + n[i2]*n[i2];
            m_tframe.col(1)[i1] = -n[i1]*n[i0];
            m_tframe.col(1)[i2] = -n[i2]*n[i0];

            // go to second pass
            m_pass = SECOND_PASS;
            return NEED_OTHER_PASS;
        }
        else
        {
            return UNDEFINED;
        }
    }
    else if(m_pass == SECOND_PASS)
    {
        // center of gravity (mean)
        m_cog /= Base::m_nbNeighbors;

        // covariance matrix
        m_cov /= Base::m_nbNeighbors;
        m_cov -= m_cog*m_cog.transpose();

        m_solver.computeDirect(m_cov);

        Base::m_k1 = m_solver.eigenvalues()(0);
        Base::m_k2 = m_solver.eigenvalues()(1);

        // transform from local plane coordinates to world coordinates
        Base::m_v1 = m_tframe * m_solver.eigenvectors().col(0);
        Base::m_v2 = m_tframe * m_solver.eigenvectors().col(1);

        //TODO(thib) which epsilon value should be chosen ?
//        Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();
        Scalar epsilon = Scalar(1e-3);
        if(Base::m_k1<epsilon && Base::m_k2<epsilon)
        {
            Base::m_k1 = Scalar(0);
            Base::m_k2 = Scalar(0);

            // set principal directions from fitted plane
            Base::m_v2 = m_tframe.col(0);
            Base::m_v1 = m_tframe.col(1);
        }

        return STABLE;
    }
    return UNDEFINED;
}
