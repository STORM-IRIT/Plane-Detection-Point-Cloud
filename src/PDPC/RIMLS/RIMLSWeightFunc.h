#pragma once

#include <Ponca/core.h>

namespace pdpc {

template<class DataPoint, class WeightKernel>
class RIMLSWeightFunc
{
public:
    using Scalar     = typename DataPoint::Scalar;
    using VectorType = typename DataPoint::VectorType;
    using MatrixType = typename DataPoint::MatrixType;

public:
    inline RIMLSWeightFunc(Scalar t = 1, Scalar sigma = 1);

public:
    inline Scalar w(const VectorType& _q, const DataPoint& attributes) const;

    inline VectorType spacedw(const VectorType& _q, const DataPoint& attributes) const;
    inline MatrixType spaced2w(const VectorType& _q, const DataPoint& attributes) const;

    inline Scalar scaledw(const VectorType& _q, const DataPoint& attributes) const;
    inline Scalar scaled2w(const VectorType& _q, const DataPoint& attributes) const;

    inline VectorType scaleSpaced2w(const VectorType& _q, const DataPoint& attributes) const;

    inline Scalar evalScale() const;

private:
    inline Scalar reweight(const VectorType& _q, const DataPoint& attributes) const;

protected:
    Ponca::DistWeightFunc<DataPoint, WeightKernel> m_weightFunc;
    Scalar m_invSigma2;
    Scalar m_invSigmaScale2;
};

} // namespace pdpc

#include <PDPC/RIMLS/RIMLSWeightFunc.hpp>
