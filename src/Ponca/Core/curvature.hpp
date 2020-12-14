/*
 Copyright (C) 2015 Gael Guennebaud <gael.guennebaud@inria.fr>
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/

template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
CurvatureEstimator<DataPoint, _WFunctor, T>::finalize()
{
    FIT_RESULT bResult = Base::finalize();

    if(bResult != UNDEFINED)
    {
        return computeCurvature(false);
    }

    return bResult;
}

template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT CurvatureEstimator<DataPoint, _WFunctor, T>::computeCurvature(bool useNormal)
{
    // Get the object space Weingarten map dN
    MatrixType dN = Base::dNormal().template middleCols<DataPoint::Dim>(Base::isScaleDer() ? 1: 0);

    // Compute tangent-space basis
    Mat32 B = tangentPlane(useNormal);

    // Compute the 2x2 matrix representing the shape operator by transforming dN to the basis B.
    // Recall that dN is a bilinear form, it thus transforms as follows:
    Mat22 S = B.transpose() * dN * B;

    // Recall that at this stage, the shape operator represented by S describes the normal curvature K_n(u) in the direction u \in R^2 as follows:
    //   K_n(u) = u^T S u
    // The principal curvatures are fully defined by the values and the directions of the extrema of K_n.
    //
    // If the normal field N(x) comes from the gradient of a scalar field, then N(x) is curl-free, and dN and S are symmetric matrices.
    // In this case, the extrema of the previous quadratic form are directly obtained by the the eigenvalue decomposition of S.
    // However, if N(x) is only an approximation of the normal field of a surface, then N(x) is not necessarily curl-free, and in this case S is not symmetric.
    // In this case, we first have to find an equivalent symmetric matrix S' such that:
    //   K_n(u) = u^T S' u,
    // for any u \in R^2.
    // It is easy to see that such a S' is simply obtained as:
    //   S' = (S + S^T)/2

    S(0,1) = S(1,0) = (S(0,1) + S(1,0))/Scalar(2);
    Eigen::SelfAdjointEigenSolver<Mat22> eig2;
    eig2.computeDirect(S);

    if (eig2.info() != Eigen::Success){
      return UNDEFINED;
    }

    m_k1 = eig2.eigenvalues()(0);
    m_k2 = eig2.eigenvalues()(1);

    m_v1 = B * eig2.eigenvectors().col(0);
    m_v2 = B * eig2.eigenvectors().col(1);

    if(abs(m_k1)<abs(m_k2))
    {
#ifdef __CUDACC__
      Scalar tmpk = m_k1;
      m_k1 = m_k2;
      m_k2 = tmpk;
      VectorType tmpv = m_v1;
      m_v1 = m_v2;
      m_v2 = tmpv;
#else
      std::swap(m_k1, m_k2);
      std::swap(m_v1, m_v2);
#endif
    }

    return this->getCurrentState();
}

template < class DataPoint, class _WFunctor, typename T>
typename CurvatureEstimator<DataPoint, _WFunctor, T>::Mat32
CurvatureEstimator<DataPoint, _WFunctor, T>::tangentPlane(bool useNormal) const
{
    typedef typename VectorType::Index Index;

    PONCA_MULTIARCH_STD_MATH(sqrt);

    Mat32 B;
    Index i0=Index(-1), i1=Index(-1), i2=Index(-1);

    // Two choices to compute a basis of the tangent plane
    if(useNormal)
    {
        // Use a vector orthogonal to the surface (the gradient) and compute an
        // orthonormal basis from it
        VectorType n = Base::primitiveGradient();
        n.array().abs().minCoeff(&i0); // i0: dimension where n extends the least
        i1 = (i0+1)%3;
        i2 = (i0+2)%3;

        B.col(0)[i0] = 0;
        B.col(0)[i1] = n[i2];
        B.col(0)[i2] = -n[i1];

        B.col(0).normalize();
        B.col(1) = B.col(0).cross(n);
    }
    else
    {
        // Use the spatial derivative of the normal. This option leads to NaN
        // values if dN is null (like in the case of a perfect plane)

        // Get the object space Weingarten map dN
        MatrixType dN = Base::dNormal().template middleCols<DataPoint::Dim>(Base::isScaleDer() ? 1: 0);

        // Compute tangent-space basis from dN
        //   1 - pick the column with maximal norm as the first tangent vector,
        Scalar sqNorm = dN.colwise().squaredNorm().maxCoeff(&i0);
        B.col(0) = dN.col(i0) / sqrt(sqNorm);
        //   2 - orthogonalize the other column vectors, and pick the most reliable one
        i1 = (i0+1)%3;
        i2 = (i0+2)%3;
        VectorType v1 = dN.col(i1) - B.col(0).dot(dN.col(i1)) * B.col(0);
        VectorType v2 = dN.col(i2) - B.col(0).dot(dN.col(i2)) * B.col(0);
        Scalar v1norm2 = v1.squaredNorm();
        Scalar v2norm2 = v2.squaredNorm();
        if(v1norm2 > v2norm2) B.col(1) = v1 / sqrt(v1norm2);
        else                  B.col(1) = v2 / sqrt(v2norm2);
    }

    return B;
}

