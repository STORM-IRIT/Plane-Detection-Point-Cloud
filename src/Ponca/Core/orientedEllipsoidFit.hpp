/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

namespace internal {

//!
//! \brief solve DX + XD = C
//! \param D a vector of size N representing a NxN diagonal matrix
//! \param C any NxN matrix
//!
template<typename MatrixType, class VectorType>
MatrixType matrix_function_solve_diagonal_sylvester(const VectorType& D, const MatrixType& C)
{
    using Index = typename VectorType::Index;
    using Scalar = typename VectorType::Scalar;

    const Index n = D.size();
    const Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();

    MatrixType X;
    for(Index i=0; i<n; ++i)
    {
        for(Index j=0; j<n; ++j)
        {
            Scalar d = D(i) + D(j);
            if(abs(d) < epsilon)
            {
                X(i,j) = Scalar(0);
            }
            else
            {
                X(i,j) = C(i,j) / d;
            }
        }
    }
    return X;
}

//!
//! \brief solve AX + XA = C
//! \param A a symmetric NxN matrix
//! \param C any NxN matrix
//!
template<typename MatrixType>
MatrixType matrix_function_solve_symmetric_sylvester(const MatrixType& A, const MatrixType& C)
{
    Eigen::SelfAdjointEigenSolver<MatrixType> eig(A);
    const auto& D = eig.eigenvalues();
    const MatrixType& P = eig.eigenvectors();
    const MatrixType Pinv = P.transpose();
    const MatrixType F = Pinv * C * P;
    const MatrixType Y = matrix_function_solve_diagonal_sylvester(D, F);
    const MatrixType X = P * Y * Pinv;
    return X;
}

} // namespace internal

template < class DataPoint, class _WFunctor, typename T>
void
OrientedEllipsoidFit<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    // Setup primitive
    Base::resetPrimitive();
    Base::basisCenter() = _evalPos;

    // Setup fitting internal values
    m_sumP     = VectorType::Zero();
    m_sumN     = VectorType::Zero();
    m_sumDotPN = Scalar(0.0);
    m_sumDotPP = Scalar(0.0);
    m_sumW     = Scalar(0.0);
    m_prodPP   = MatrixType::Zero();
    m_prodPN   = MatrixType::Zero();
}

template < class DataPoint, class _WFunctor, typename T>
bool
OrientedEllipsoidFit<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    // centered basis
    VectorType q = _nei.pos() - Base::basisCenter();

    // compute weight
    Scalar w = m_w.w(q, _nei);

    if (w > Scalar(0.))
    {
        // increment matrix
        m_sumP     += w * q;
        m_sumN     += w * _nei.normal() * w;
        m_sumDotPN += w * _nei.normal().dot(q);
        m_sumDotPP += w * q.squaredNorm();
        m_sumW     += w;
        m_prodPP   += w * q * q.transpose();
        m_prodPN   += w * q * _nei.normal().transpose();

        /*! \todo Handle add of multiple similar neighbors (maybe user side)*/
        ++(Base::m_nbNeighbors);
        return true;
    }

    return false;
}


template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
OrientedEllipsoidFit<DataPoint, _WFunctor, T>::finalize ()
{
    PONCA_MULTIARCH_STD_MATH(sqrt);
    PONCA_MULTIARCH_STD_MATH(max);
    PONCA_MULTIARCH_STD_MATH(abs);

    // 1. finalize ellipsoid fitting
    Scalar epsilon = Eigen::NumTraits<Scalar>::dummy_precision();

    // handle specific configurations
    // With less than 3 neighbors the fitting is undefined
    if(m_sumW == Scalar(0.) || Base::m_nbNeighbors < 3)
    {
        Base::m_ul.setZero();
        Base::m_uc = Scalar(0.);
        Base::m_uq.setZero();
        Base::m_isNormalized = false;
        Base::m_eCurrentState = UNDEFINED;
        return Base::m_eCurrentState;
    }

    Scalar invSumW = Scalar(1.)/m_sumW;

//    MatrixType A = Scalar(2) * m_sumW * m_prodPP - Scalar(4) * m_sumP * m_sumP.transpose();
    MatrixType A = Scalar(2) * (m_sumW * m_prodPP - m_sumP * m_sumP.transpose());
    MatrixType C = m_sumW * m_prodPN - m_sumP * m_sumN.transpose();
    C = C + C.transpose().eval();

    Base::m_uq = internal::matrix_function_solve_symmetric_sylvester(A, C);
    Base::m_ul = invSumW * (m_sumN - Scalar(2) * Base::m_uq * m_sumP);
    Base::m_uc = - invSumW * ( Base::m_ul.dot(m_sumP) + (m_prodPP * Base::m_uq).trace() );

    Base::m_isNormalized = false;

    if(Base::m_nbNeighbors < 6)
    {
        Base::m_eCurrentState = UNSTABLE;
    }
    else
    {
        Base::m_eCurrentState = STABLE;
    }

    return Base::m_eCurrentState;
}

namespace internal
{

template < class DataPoint, class _WFunctor, typename T, int Type>
void
OrientedEllipsoidDer<DataPoint, _WFunctor, T, Type>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_dSumN     = VectorArray::Zero();
    m_dSumP     = VectorArray::Zero();

    m_dSumDotPN = ScalarArray::Zero();
    m_dSumDotPP = ScalarArray::Zero();
    m_dSumW     = ScalarArray::Zero();
    m_dNume     = ScalarArray::Zero();
    m_dDeno     = ScalarArray::Zero();

    m_dUc       = ScalarArray::Zero();
    m_dUq       = ScalarArray::Zero();
    m_dUl       = VectorArray::Zero();
}


template < class DataPoint, class _WFunctor, typename T, int Type>
bool
OrientedEllipsoidDer<DataPoint, _WFunctor, T, Type>::addNeighbor(const DataPoint  &_nei)
{
    bool bResult = Base::addNeighbor(_nei);

    if(bResult)
    {
        ScalarArray dw;

        // centered basis
        VectorType q = _nei.pos() - Base::basisCenter();

        // compute weight derivatives
        if (Type & FitScaleDer)
            dw[0] = Base::m_w.scaledw(q, _nei);

        if (Type & FitSpaceDer)
            dw.template tail<int(DataPoint::Dim)>() = -Base::m_w.spacedw(q, _nei).transpose();

        // increment
        m_dSumW     += dw;
        m_dSumP     += q * dw;
        m_dSumN     += _nei.normal() * dw;
        m_dSumDotPN += dw * _nei.normal().dot(q);
        m_dSumDotPP += dw * q.squaredNorm();

        return true;
    }

    return false;
}


template < class DataPoint, class _WFunctor, typename T, int Type>
FIT_RESULT
OrientedEllipsoidDer<DataPoint, _WFunctor, T, Type>::finalize()
{
    PONCA_MULTIARCH_STD_MATH(sqrt);

    Base::finalize();
    // Test if base finalize end on a viable case (stable / unstable)
    if (this->isReady())
    {
        Scalar invSumW = Scalar(1.)/Base::m_sumW;

        Scalar nume  = Base::m_sumDotPN - invSumW*Base::m_sumP.dot(Base::m_sumN);
        Scalar deno  = Base::m_sumDotPP - invSumW*Base::m_sumP.dot(Base::m_sumP);

        // FIXME, the following product "Base::m_sumN.transpose() * m_dSumP" is prone to numerical cancellation
        // issues for spacial derivatives because, (d sum w_i P_i)/(d x) is supposed to be tangent to the surface whereas
        // "sum w_i N_i" is normal to the surface...
        m_dNume = m_dSumDotPN
            - invSumW*invSumW * ( Base::m_sumW * ( Base::m_sumN.transpose() * m_dSumP + Base::m_sumP.transpose() * m_dSumN )
            - m_dSumW*Base::m_sumP.dot(Base::m_sumN) );

        m_dDeno = m_dSumDotPP
            - invSumW*invSumW*(   Scalar(2.) * Base::m_sumW * Base::m_sumP.transpose() * m_dSumP
            - m_dSumW*Base::m_sumP.dot(Base::m_sumP) );

        m_dUq =  Scalar(.5) * (deno * m_dNume - m_dDeno * nume)/(deno*deno);

        // FIXME: this line is prone to numerical cancellation issues because dSumN and u_l*dSumW are close to each other.
        // If using two passes, one could directly compute sum( dw_i + (n_i - ul) ) to avoid this issue.
        m_dUl =  invSumW * ( m_dSumN - Base::m_ul*m_dSumW - Scalar(2.)*(m_dSumP*Base::m_uq + Base::m_sumP*m_dUq) );
        m_dUc = -invSumW*( Base::m_sumP.transpose() * m_dUl
            + Base::m_sumDotPP * m_dUq
            + Base::m_ul.transpose() * m_dSumP
            + Base::m_uq * m_dSumDotPP
            + m_dSumW * Base::m_uc);
    }

    return Base::m_eCurrentState;
}

template < class DataPoint, class _WFunctor, typename T, int Type>
typename OrientedEllipsoidDer <DataPoint, _WFunctor, T, Type>::VectorArray
OrientedEllipsoidDer<DataPoint, _WFunctor, T, Type>::dNormal() const
{
  // Computes the derivatives of the normal of the ellipsoid at the evaluation point.
  // Therefore, we must take into account the variation of the evaluation point when differentiating wrt space
  // i.e., normal(x) = grad / |grad|, with grad(x) = ul + 2 uq * x, and diff_x(grad) = dul + 2 uq I
  VectorArray dgrad = m_dUl;
  if(this->isSpaceDer())
    dgrad.template rightCols<DataPoint::Dim>().diagonal().array() += Scalar(2)*Base::m_uq;
  Scalar norm  = Base::m_ul.norm();
  Scalar norm3 = norm*norm*norm;
  return dgrad / norm - Base::m_ul * (Base::m_ul.transpose() * dgrad) / norm3;
}

template < class DataPoint, class _WFunctor, typename T, int Type>
typename OrientedEllipsoidDer <DataPoint, _WFunctor, T, Type>::ScalarArray
OrientedEllipsoidDer<DataPoint, _WFunctor, T, Type>::dPotential() const
{
  ScalarArray dfield = m_dUc;
  if(this->isSpaceDer())
    dfield.template tail<DataPoint::Dim>() += Base::m_ul;
  return dfield;
}

template < class DataPoint, class _WFunctor, typename T, int Type>
bool
OrientedEllipsoidDer<DataPoint, _WFunctor, T, Type>::applyPrattNorm()
{
    if(Base::isNormalized())
        return false; //need original parameters without Pratt Normalization

    PONCA_MULTIARCH_STD_MATH(sqrt);
    Scalar pn2    = Base::prattNorm2();
    Scalar pn     = sqrt(pn2);

    ScalarArray dpn2   = dprattNorm2();
    ScalarArray factor = Scalar(0.5) * dpn2 / pn;

    m_dUc = ( m_dUc * pn - Base::m_uc * factor ) / pn2;
    m_dUl = ( m_dUl * pn - Base::m_ul * factor ) / pn2;
    m_dUq = ( m_dUq * pn - Base::m_uq * factor ) / pn2;

    Base::applyPrattNorm();
    return true;
}

}// namespace internal
