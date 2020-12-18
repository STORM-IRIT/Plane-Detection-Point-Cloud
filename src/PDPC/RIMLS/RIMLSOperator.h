#pragma once

#include <PDPC/Common/Defines.h>

#include <PDPC/RIMLS/RIMLSWeightFunc.h>
#include <PDPC/RIMLS/RIMLSPoint.h>

#include <memory>

namespace pdpc {

class KdTreeRangePointQuery;
class PointCloud;

// =============================================================================

class RIMLSOperator
{
    // Types -------------------------------------------------------------------
public:
    using Point         = RIMLSPoint<Vector3>;
    using WeightKernel  = Ponca::SmoothWeightKernel<Scalar>;
    using WeightFunc    = RIMLSWeightFunc<Point, WeightKernel>;
    using FitStep       = Ponca::Basket<Point, WeightFunc, Ponca::OrientedSphereFit>;
    using FitFinal      = Ponca::Basket<Point, WeightFunc, Ponca::OrientedSphereFit,
                                                           Ponca::OrientedSphereScaleSpaceDer,
                                                           Ponca::MlsSphereFitDer,
                                                           Ponca::CurvatureEstimator>;

    // MLSOperator -------------------------------------------------------------
public:
    RIMLSOperator();
    RIMLSOperator(const RIMLSOperator& other);
    ~RIMLSOperator();

    void compute(const PointCloud& points, Vector3& point);

    // Parameters --------------------------------------------------------------
public:
    void set_scale(Scalar scale);
    void set_step_max(int step_max);
    void set_convergence_ratio_min(Scalar convergence_ratio_min);

    void set_reweighting_sigma(Scalar sigma);
    void set_reweighting_step(int step);

    // Accessors ---------------------------------------------------------------
public:
    bool  stable() const;
    const FitFinal& fit() const;
          FitFinal& fit();

    // Data --------------------------------------------------------------------
protected:
    Scalar      m_scale;

    Scalar      m_reweighting_sigma; //TODO this is never used...
    int         m_reweighting_step;

    int         m_step_max;
    Scalar      m_convergence_ratio_min;

    int         m_step;
    bool        m_stable;
    int         m_neighbor_count;

    WeightFunc  m_weight_func;
    FitStep     m_fit_step;
    FitFinal    m_fit_final;

    std::unique_ptr<KdTreeRangePointQuery> m_query;
};

} // namespace pdpc
