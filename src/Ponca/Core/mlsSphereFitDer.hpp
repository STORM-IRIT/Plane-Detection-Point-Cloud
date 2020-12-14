/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/


template < class DataPoint, class _WFunctor, typename T>
void
MlsSphereFitDer<DataPoint, _WFunctor, T>::init(const VectorType& _evalPos)
{
    Base::init(_evalPos);

    m_d2Uc = Matrix::Zero(),
    m_d2Uq = Matrix::Zero();
    m_d2Ul = MatrixArray::Zero();

    m_d2SumDotPN = Matrix::Zero();
    m_d2SumDotPP = Matrix::Zero();
    m_d2SumW     = Matrix::Zero();

    m_d2SumP = MatrixArray::Zero();
    m_d2SumN = MatrixArray::Zero();
}

template < class DataPoint, class _WFunctor, typename T>
bool
MlsSphereFitDer<DataPoint, _WFunctor, T>::addNeighbor(const DataPoint& _nei)
{
    bool bResult = Base::addNeighbor(_nei);

    if(bResult)
    {
        // centered basis
        VectorType q = _nei.pos() - Base::basisCenter();

        // compute weight derivatives
        Matrix d2w = Matrix::Zero();

        if (Base::isScaleDer())
            d2w(0,0) = Base::m_w.scaled2w(q, _nei);

        if (Base::isSpaceDer())
            d2w.template bottomRightCorner<Dim,Dim>() = Base::m_w.spaced2w(q, _nei);

        if (Base::isScaleDer() && Base::isSpaceDer())
        {
            d2w.template bottomLeftCorner<Dim,1>() = Base::m_w.scaleSpaced2w(q,_nei);
            d2w.template topRightCorner<1,Dim>() = d2w.template bottomLeftCorner<Dim,1>().transpose();
        }

        m_d2SumDotPN += d2w * _nei.normal().dot(q);
        m_d2SumDotPP += d2w * q.squaredNorm();
        m_d2SumW     += d2w;

        for(int i=0; i<Dim; ++i)
        {
            m_d2SumP.template block<DerDim,DerDim>(0,i*DerDim) += d2w * q[i];
            m_d2SumN.template block<DerDim,DerDim>(0,i*DerDim) += d2w * _nei.normal()[i];
        }
    }
    return bResult;
}

template < class DataPoint, class _WFunctor, typename T>
FIT_RESULT
MlsSphereFitDer<DataPoint, _WFunctor, T>::finalize()
{
    Base::finalize();

    if (this->isReady())
    {
        Matrix sumdSumPdSumN  = Matrix::Zero();
        Matrix sumd2SumPdSumN = Matrix::Zero();
        Matrix sumd2SumNdSumP = Matrix::Zero();
        Matrix sumdSumPdSumP  = Matrix::Zero();
        Matrix sumd2SumPdSumP = Matrix::Zero();

        for(int i=0; i<Dim; ++i)
        {
            sumdSumPdSumN  += Base::m_dSumN.row(i).transpose()*Base::m_dSumP.row(i);
            sumd2SumPdSumN += m_d2SumP.template block<DerDim,DerDim>(0,i*DerDim)*Base::m_sumN(i);
            sumd2SumNdSumP += m_d2SumN.template block<DerDim,DerDim>(0,i*DerDim)*Base::m_sumP(i);
            sumdSumPdSumP  += Base::m_dSumP.row(i).transpose()*Base::m_dSumP.row(i);
            sumd2SumPdSumP += m_d2SumP.template block<DerDim,DerDim>(0,i*DerDim)*Base::m_sumP(i);
        }

        Scalar invSumW = Scalar(1.)/Base::m_sumW;

        Matrix d2Nume = m_d2SumDotPN
            - invSumW*invSumW*invSumW*invSumW*(
                    Base::m_sumW*Base::m_sumW*(  Base::m_sumW*(sumdSumPdSumN+sumdSumPdSumN.transpose()+sumd2SumPdSumN+sumd2SumNdSumP)
                                               + Base::m_dSumW.transpose()*(Base::m_sumN.transpose()*Base::m_dSumP + Base::m_sumP.transpose()*Base::m_dSumN)
                                               - (Base::m_sumP.transpose()*Base::m_sumN)*m_d2SumW.transpose()
                                               - (Base::m_dSumN.transpose()*Base::m_sumP + Base::m_dSumP.transpose()*Base::m_sumN)*Base::m_dSumW)
                    - Scalar(2.)*Base::m_sumW*Base::m_dSumW.transpose()*(Base::m_sumW*(Base::m_sumN.transpose()*Base::m_dSumP + Base::m_sumP.transpose()*Base::m_dSumN)
                                                                         - (Base::m_sumP.transpose()*Base::m_sumN)*Base::m_dSumW));

        Matrix d2Deno = m_d2SumDotPP
            - invSumW*invSumW*invSumW*invSumW*(
                Base::m_sumW*Base::m_sumW*(  Scalar(2.)*Base::m_sumW*(sumdSumPdSumP+sumd2SumPdSumP)
                                           + Scalar(2.)*Base::m_dSumW.transpose()*(Base::m_sumP.transpose()*Base::m_dSumP)
                                           - (Base::m_sumP.transpose()*Base::m_sumP)*m_d2SumW.transpose()
                                           - Scalar(2.)*(Base::m_dSumP.transpose()*Base::m_sumP)*Base::m_dSumW)
                - Scalar(2.)*Base::m_sumW*Base::m_dSumW.transpose()*(Scalar(2.)*Base::m_sumW*Base::m_sumP.transpose()*Base::m_dSumP
                                                                     - (Base::m_sumP.transpose()*Base::m_sumP)*Base::m_dSumW));

        Scalar deno2 = Base::m_deno*Base::m_deno;

        m_d2Uq = Scalar(.5)/(deno2*deno2)*(deno2*(  Base::m_dDeno.transpose()*Base::m_dNume
                                                  + Base::m_deno*d2Nume
                                                  - Base::m_dNume.transpose()*Base::m_dDeno
                                                  - Base::m_nume*d2Deno)
                                           - Scalar(2.)*Base::m_deno*Base::m_dDeno.transpose()*(Base::m_deno*Base::m_dNume - Base::m_nume*Base::m_dDeno));

        for(int i=0; i<Dim; ++i)
        {
            m_d2Ul.template block<DerDim,DerDim>(0,i*DerDim) = invSumW*(
                  m_d2SumN.template block<DerDim,DerDim>(0,i*DerDim)
                - Scalar(2.)*(  m_d2Uq*Base::m_sumP[i]
                              + Base::m_dSumP.row(i).transpose()*Base::m_dUq
                              + Base::m_uq*m_d2SumP.template block<DerDim,DerDim>(0,i*DerDim)
                              + Base::m_dUq.transpose()*Base::m_dSumP.row(i))
                - Base::m_ul[i]*m_d2SumW
                - Base::m_dUl.row(i).transpose()*Base::m_dSumW
                - Base::m_dSumW.transpose()*Base::m_dUl.row(i));
        }
        
        Matrix sumdUldSumP = Matrix::Zero();
        Matrix sumUld2SumP = Matrix::Zero();
        Matrix sumd2UlsumP = Matrix::Zero();
        Matrix sumdSumPdUl = Matrix::Zero();

        for(int i=0; i<Dim; ++i)
        {
            sumdUldSumP += Base::m_dUl.row(i).transpose()*Base::m_dSumP.row(i);
            sumUld2SumP += Base::m_ul[i]*m_d2SumP.template block<DerDim,DerDim>(0,i*DerDim);
            sumd2UlsumP += m_d2Ul.template block<DerDim,DerDim>(0,i*DerDim)*Base::m_sumP[i];
            sumdSumPdUl += Base::m_dSumP.row(i).transpose()*Base::m_dUl.row(i);
        }

        m_d2Uc = -invSumW*(
              sumdUldSumP
            + sumUld2SumP
            + sumd2UlsumP
            + sumdSumPdUl
            + Base::m_dUq.transpose()*Base::m_dSumDotPP
            + Base::m_uq*m_d2SumDotPP
            + Base::m_dSumDotPP.transpose()*Base::m_dUq
            + m_d2Uq*Base::m_sumDotPP
            + Base::m_uc*m_d2SumW
            + Base::m_dUc.transpose()*Base::m_dSumW
            + Base::m_dSumW.transpose()*Base::m_dUc);
    }

    return Base::m_eCurrentState;
}

template < class DataPoint, class _WFunctor, typename T>
typename MlsSphereFitDer<DataPoint, _WFunctor, T>::ScalarArray
MlsSphereFitDer<DataPoint, _WFunctor, T>::dPotential() const
{
    // Compute the 1st order derivative of the scalar field s = uc + x^T ul + x^T x uq
    // In a centered basis (x=0), we obtain:
    //   the scale derivative:   d_t(s)(t,0) = d_t(uc)(t,0)
    //   the spatial derivative: d_x(s)(t,0) = d_x(uc)(t,0) + ul(t,0)
    ScalarArray result = Base::m_dUc;

    if(Base::isSpaceDer())
        result.template tail<Dim>() += Base::m_ul;

    return result;
}

template < class DataPoint, class _WFunctor, typename T>
typename MlsSphereFitDer<DataPoint, _WFunctor, T>::VectorType
MlsSphereFitDer<DataPoint, _WFunctor, T>::normal() const
{
    // Compute the 1st order derivative of the scalar field and normalize it
    VectorType grad = Base::m_dUc.template tail<Dim>().transpose() + Base::m_ul;
    return grad.normalized();
}

template < class DataPoint, class _WFunctor, typename T>
typename MlsSphereFitDer<DataPoint, _WFunctor, T>::VectorArray
MlsSphereFitDer<DataPoint, _WFunctor, T>::dNormal() const
{
    // Compute the 1st order derivative of the normal, which is the normalized gradient N = d_x(s) / |d_x(s)|
    // We obtain:
    //   the scale derivative:   d_t(N) = (I-N N^T) / |d_x(s)| d2_tx(s)
    //   the spatial derivative: d_x(N) = (I-N N^T) / |d_x(s)| d2_x2(s)
    // Where in a centered basis (x=0), we have:
    //   d2_tx(s) = d2_tx(uc) + d_t(ul)
    //   d2_x2(s) = d2_x2(uc) + d_x(ul) + d_x(ul)^T + 2 uq I
    VectorArray result = VectorArray::Zero();

    VectorType grad = Base::m_dUc.template tail<Dim>().transpose() + Base::m_ul;
    Scalar gradNorm = grad.norm();

    if(Base::isScaleDer())
        result.col(0) = m_d2Uc.template topRightCorner<1,Dim>().transpose() + Base::m_dUl.col(0);

    if(Base::isSpaceDer())
    {
        result.template rightCols<Dim>() = m_d2Uc.template bottomRightCorner<Dim,Dim>().transpose()
                                           + Base::m_dUl.template rightCols<Dim>().transpose()
                                           + Base::m_dUl.template rightCols<Dim>();
        result.template rightCols<Dim>().diagonal().array() += Scalar(2.)*Base::m_uq;
    }

    return result/gradNorm - grad*grad.transpose()/(gradNorm*gradNorm)*result;
}

template < class DataPoint, class _WFunctor, typename T>
typename MlsSphereFitDer<DataPoint, _WFunctor, T>::Matrix
MlsSphereFitDer<DataPoint, _WFunctor, T>::hessian() const
{
    // Compute the 2nd order derivative of the scalar field (in centered basis):
    //   the spatial derivative: d2_x2(s) = d2_x2(uc) + d_x(ul) + d_x(ul)^T + 2 uq I
    //   the cross derivatives:  d2_xt(s) = d2_xt(uc) + d_t(ul)
    //   the scale derivative:   d2_t2(s) = d2_t2(uc)

    Matrix hessian = m_d2Uc;

    if(Base::isSpaceDer())
    {
        hessian.template bottomRightCorner<Dim,Dim>() += Base::m_dUl.template bottomRightCorner<Dim,Dim>() +
                                                         Base::m_dUl.template bottomRightCorner<Dim,Dim>().transpose();
        hessian.template bottomRightCorner<Dim,Dim>().diagonal().array() += Scalar(2) * Base::m_uq;

        if(Base::isScaleDer())
        {
            hessian.template bottomLeftCorner<Dim,1>() += Base::m_dUl.template bottomLeftCorner<Dim,1>();
            hessian.template topRightCorner<1,Dim>() = hessian.template bottomLeftCorner<Dim,1>().transpose();
        }
    }

    return hessian;
}
