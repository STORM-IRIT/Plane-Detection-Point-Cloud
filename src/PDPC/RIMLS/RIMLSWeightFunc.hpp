#include <PDPC/RIMLS/RIMLSWeightFunc.h>

namespace pdpc {

template<class P, class W>
RIMLSWeightFunc<P,W>::RIMLSWeightFunc(Scalar t, Scalar sigma) :
    m_weightFunc(t),
    m_invSigma2(Scalar(1)/(sigma*sigma)),
    m_invSigmaScale2(Scalar(1)/(0.5*t * 0.5*t))
{
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::Scalar
RIMLSWeightFunc<P,W>::w(const VectorType& _q, const P& attributes) const
{
    return reweight(_q, attributes) * m_weightFunc.w(_q, attributes);
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::VectorType
RIMLSWeightFunc<P,W>::spacedw(const VectorType& _q, const P& attributes) const
{
    return reweight(_q, attributes) * m_weightFunc.spacedw(_q, attributes);
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::MatrixType
RIMLSWeightFunc<P,W>::spaced2w(const VectorType& _q, const P& attributes) const
{
    return reweight(_q, attributes) * m_weightFunc.spaced2w(_q, attributes);
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::Scalar
RIMLSWeightFunc<P,W>::scaledw(const VectorType& _q, const P& attributes) const
{
    return reweight(_q, attributes) * m_weightFunc.scaledw(_q, attributes);
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::Scalar
RIMLSWeightFunc<P,W>::scaled2w(const VectorType& _q, const P& attributes) const
{
    return reweight(_q, attributes) * m_weightFunc.scaled2w(_q, attributes);
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::VectorType
RIMLSWeightFunc<P,W>::scaleSpaced2w(const VectorType& _q, const P& attributes) const
{
    return reweight(_q, attributes) * m_weightFunc.scaleSpaced2w(_q, attributes);
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::Scalar
RIMLSWeightFunc<P,W>::evalScale() const
{
    return m_weightFunc.evalScale();
}

template<class P, class W>
typename RIMLSWeightFunc<P,W>::Scalar
RIMLSWeightFunc<P,W>::reweight(const VectorType& /*_q*/, const P& attributes) const
{
    return exp(-attributes.gradientDiff()  * attributes.gradientDiff()  * m_invSigma2
               -attributes.potentialDiff() * attributes.potentialDiff() * m_invSigmaScale2);
}

} // namespace pdpc
