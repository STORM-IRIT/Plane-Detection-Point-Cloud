/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/



template <class DataPoint, class WeightKernel>
typename DistWeightFunc<DataPoint, WeightKernel>::Scalar
DistWeightFunc<DataPoint, WeightKernel>::w( const VectorType& _q, 
					                        const DataPoint&) const
{
    Scalar d  = _q.norm();  
    return (d <= m_t) ? m_wk.f(d/m_t) : Scalar(0.);
}

template <class DataPoint, class WeightKernel>
typename DistWeightFunc<DataPoint, WeightKernel>::VectorType
DistWeightFunc<DataPoint, WeightKernel>::spacedw(   const VectorType& _q, 
						                            const DataPoint&) const
{
    VectorType result = VectorType::Zero();
    Scalar d = _q.norm();
    if (d <= m_t && d != Scalar(0.)) result = (_q / (d * m_t)) * m_wk.df(d/m_t);
    return result;
}

template <class DataPoint, class WeightKernel>
typename DistWeightFunc<DataPoint, WeightKernel>::MatrixType
DistWeightFunc<DataPoint, WeightKernel>::spaced2w(   const VectorType& _q,
                                                     const DataPoint&) const
{
    MatrixType result = MatrixType::Zero();
    Scalar d = _q.norm();
    if (d <= m_t && d != Scalar(0.))
    {
        Scalar der = m_wk.df(d/m_t);
        result = _q*_q.transpose()/d*(m_wk.ddf(d/m_t)/m_t - der/d);
        result.diagonal().array() += der;
        result *= Scalar(1.)/(m_t*d);
    }
    return result;
}

template <class DataPoint, class WeightKernel>
typename DistWeightFunc<DataPoint, WeightKernel>::Scalar
DistWeightFunc<DataPoint, WeightKernel>::scaledw(   const VectorType& _q, 
						                            const DataPoint&) const
{
    Scalar d  = _q.norm();  
    return (d <= m_t) ? Scalar( - d*m_wk.df(d/m_t)/(m_t*m_t) ) : Scalar(0.);
}

template <class DataPoint, class WeightKernel>
typename DistWeightFunc<DataPoint, WeightKernel>::Scalar
DistWeightFunc<DataPoint, WeightKernel>::scaled2w(   const VectorType& _q,
                                                     const DataPoint&) const
{
    Scalar d  = _q.norm();
    return (d <= m_t) ? Scalar(Scalar(2.)*d/(m_t*m_t*m_t)*m_wk.df(d/m_t) +
                               d*d/(m_t*m_t*m_t*m_t)*m_wk.ddf(d/m_t)) :
                        Scalar(0.);
}

template <class DataPoint, class WeightKernel>
typename DistWeightFunc<DataPoint, WeightKernel>::VectorType
DistWeightFunc<DataPoint, WeightKernel>::scaleSpaced2w(   const VectorType& _q,
                                                          const DataPoint&) const
{
    VectorType result = VectorType::Zero();
    Scalar d = _q.norm();
    if (d <= m_t && d != Scalar(0.))
        result = -_q/(m_t*m_t)*(m_wk.df(d/m_t)/d + m_wk.ddf(d/m_t)/m_t);
    return result;
}


