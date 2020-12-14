/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


template < class DataPoint, class _WFunctor, typename T>
void
SphereFit<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    // Setup primitive
    Base::resetPrimitive();
    Base::basisCenter() = _evalPos;

    // Setup fitting internal values
    m_sumW = Scalar(0.0);
    m_matA.setZero();
}

template < class DataPoint, class _WFunctor, typename T>
bool
SphereFit<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    // centered basis
    VectorType q = _nei.pos() - Base::basisCenter();

    // compute weight
    Scalar w = m_w.w(q, _nei);

    if (w > Scalar(0.))
    {
        VectorA a;
#ifdef __CUDACC__
        a(0) = 1;
        a.template segment<DataPoint::Dim>(1) = q;
        a(DataPoint::Dim+1) = q.squaredNorm();
#else
        a << 1, q, q.squaredNorm();
#endif
        m_matA     += w * a * a.transpose();
        m_sumW     += w;

        ++(Base::m_nbNeighbors);
        return true;
    }

    return false;
}


template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
SphereFit<DataPoint, _WFunctor, T>::finalize ()
{
    // handle specific configurations
    // With less than 3 neighbors the fitting is undefined
    if(m_sumW == Scalar(0.) || Base::m_nbNeighbors < 3)
    {
        Base::m_ul.setZero();
        Base::m_uc = Scalar(0.);
        Base::m_uq = Scalar(0.);
        Base::m_isNormalized = false;
        Base::m_eCurrentState = UNDEFINED;
        return Base::m_eCurrentState;
    }

    MatrixA matC;
    matC.setIdentity();
    matC.template topRightCorner<1,1>()    << -2;
    matC.template bottomLeftCorner<1,1>()  << -2;
    matC.template topLeftCorner<1,1>()     << 0;
    matC.template bottomRightCorner<1,1>() << 0;

    MatrixA invCpratt;
    invCpratt.setIdentity();
    invCpratt.template topRightCorner<1,1>()    << -0.5;
    invCpratt.template bottomLeftCorner<1,1>()  << -0.5;
    invCpratt.template topLeftCorner<1,1>()     << 0;
    invCpratt.template bottomRightCorner<1,1>() << 0;

    MatrixA M = invCpratt * m_matA;
    // go to positive semi-definite matrix to be compatible with
    // SelfAdjointEigenSolver requirements
    // Note: This does not affect the eigen vectors order
    Eigen::SelfAdjointEigenSolver<MatrixA> solver;
#ifdef __CUDACC__
    solver.computeDirect(M.transpose() * M);
#else
    solver.compute(M.transpose() * M);
#endif
    VectorA eivals = solver.eigenvalues().real();
    int minId = -1;
    for(int i=0 ; i<DataPoint::Dim+2 ; ++i)
    {
    Scalar ev = eivals(i);
    if((ev>0) && (minId==-1 || ev<eivals(minId)))
    minId = i;
    }

    //mLambda = eivals(minId);
    VectorA vecU = solver.eigenvectors().col(minId).real();
    Base::m_uq = vecU[1+DataPoint::Dim];
    Base::m_ul = vecU.template segment<DataPoint::Dim>(1);
    Base::m_uc = vecU[0];

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
