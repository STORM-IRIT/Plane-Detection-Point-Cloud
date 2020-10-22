#include <PDPC/RIMLS/RIMLSOperator.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/SpacePartitioning/KdTree.h>

namespace pdpc {

RIMLSOperator::RIMLSOperator() :
    m_scale(1.0),
    m_reweighting_sigma(1),
    m_reweighting_step(1),
    m_step_max(100),
    m_convergence_ratio_min(0.01),
    m_step(0),
    m_stable(false),
    m_neighbor_count(0),
    m_weight_func(1.0),
    m_fit_step(),
    m_fit_final(),
    m_query(std::make_unique<KdTreeRangePointQuery>())
{
}

RIMLSOperator::~RIMLSOperator()
{
}

RIMLSOperator::RIMLSOperator(const RIMLSOperator& other) :
    m_scale(other.m_scale),
    m_reweighting_sigma(other.m_reweighting_sigma),
    m_reweighting_step(other.m_reweighting_step),
    m_step_max(other.m_step_max),
    m_convergence_ratio_min(other.m_convergence_ratio_min),
    m_step(other.m_step),
    m_stable(other.m_stable),
    m_neighbor_count(other.m_neighbor_count),
    m_weight_func(other.m_weight_func),
    m_fit_step(other.m_fit_step),
    m_fit_final(other.m_fit_final),
    m_query(std::make_unique<KdTreeRangePointQuery>())
{
}

void RIMLSOperator::compute(const PointCloud& points, Vector3& point)
{
    *m_query = points.kdtree().range_point_query(m_scale);

    // First steps -------------------------------------------------------------
    m_fit_step.setWeightFunc(m_weight_func);
    Scalar dist_min = m_convergence_ratio_min * m_scale;
    Scalar dist     = 10 * dist_min; // arbitrary initialization > dist_min
    m_step          = 0;

    Scalar  uc       = Scalar(0);
    Vector3 ul       = Vector3::Zero();
    Scalar  uq       = Scalar(0);

    Scalar  potential = Scalar(0);
    Vector3 gradient  = Vector3::Zero();

    m_stable        = true;
    bool converge   = dist < dist_min;
    bool reach_max  = m_step >= m_step_max-1;

    while(m_stable && !reach_max && !converge)
    {
        // Reweighting
        for(int n=0; n<m_reweighting_step; ++n)
        {
            // init
            m_fit_step.init(point);

            // add neighbors
            m_query->set_point(point);
            for(int idx_nei : *m_query)
            {
                Scalar diffN = Scalar(0.);
                Scalar diffP = Scalar(0.);
                if(n>0)
                {
                    Vector3 q = points.point(idx_nei) - point;
                    Scalar  s = uc + q.dot(ul) + q.squaredNorm()*uq;
                    diffN = (gradient - points.normal(idx_nei)).norm();
                    diffP = potential-s;
                }
                Point pt(points.point(idx_nei),
                         points.normal(idx_nei),
                         diffN,
                         diffP);
                m_fit_step.addNeighbor(pt);
            }

            // finalize
            m_stable = (m_fit_step.finalize() == Ponca::STABLE);
            if(m_stable)
            {
                uc = m_fit_step.m_uc;
                ul = m_fit_step.m_ul;
                uq = m_fit_step.m_uq;
                potential = m_fit_step.m_uc;
                gradient  = m_fit_step.m_ul;
            }
        }

        // project
        if(m_stable)
        {
            Vector3 proj = m_fit_step.project(point);
            dist         = (proj-point).norm();
            converge     = dist < dist_min;
            point        = proj;
        }

        // increment
        ++m_step;
        reach_max = m_step >= m_step_max-1;
    }

    // Last step ---------------------------------------------------------------
    if(m_stable)
    {
        // init
        m_fit_final.setWeightFunc(m_weight_func);
        m_fit_final.init(point);
        m_neighbor_count = 0;

        // add neighbors
        m_query->set_point(point);
        for(int idx_nei : *m_query)
        {
            Scalar diffN = Scalar(0.);
            Scalar diffP = Scalar(0.);
            if(m_step > 0 && m_reweighting_step > 1)
            {
                Vector3 q = points.point(idx_nei) - point;
                Scalar  s = uc + q.dot(ul) + q.squaredNorm()*uq;
                diffN = (gradient - points.normal(idx_nei)).norm();
                diffP = potential-s;
            }
            Point pt(points.point(idx_nei),
                     points.normal(idx_nei),
                     diffN,
                     diffP);
            m_fit_final.addNeighbor(pt);
            ++m_neighbor_count;
        }

        // finalize
        m_stable = (m_fit_final.finalize() == Ponca::STABLE);
        if(m_stable)
        {
            point = m_fit_final.project(point);
            m_fit_final.computeCurvature(true); // useNormal = true
        }
        ++m_step;
    }
}

// Accessors -------------------------------------------------------------------


bool RIMLSOperator::stable() const
{
    return m_stable;
}


const typename RIMLSOperator::FitFinal& RIMLSOperator::fit() const
{
    return m_fit_final;
}


typename RIMLSOperator::FitFinal& RIMLSOperator::fit()
{
    return m_fit_final;
}

// Parameters ------------------------------------------------------------------


void RIMLSOperator::set_scale(Scalar scale)
{
    m_weight_func = WeightFunc(scale);
    m_scale = scale;
}


void RIMLSOperator::set_step_max(int step_max)
{
    m_step_max = step_max;
}


void RIMLSOperator::set_convergence_ratio_min(Scalar convergence_ratio_min)
{
    m_convergence_ratio_min = convergence_ratio_min;
}


void RIMLSOperator::set_reweighting_sigma(Scalar sigma)
{
    m_reweighting_sigma = sigma;
}


void RIMLSOperator::set_reweighting_step(int step)
{
    m_reweighting_step= step;
}

} // namespace pdpc
