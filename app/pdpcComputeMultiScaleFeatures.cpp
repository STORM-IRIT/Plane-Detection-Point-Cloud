#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/SpacePartitioning/KdTree.h>
#include <PDPC/ScaleSpace/ScaleSampling.h>
#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>
#include <PDPC/RIMLS/RIMLSOperator.h>

#include <algorithm>
#include <numeric>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input  = opt.get_string("input",  "i").set_required()       .set_brief("Input point cloud (.ply/.obj)");
    const std::string in_output = opt.get_string("output", "o").set_default("output").set_brief("Output name");

    const Scalar in_scount = opt.get_float("scale_count", "scount").set_default(50).set_brief("Scale count");
    const Scalar in_smin   = opt.get_float("scale_min",   "smin"  ).set_default(1) .set_brief("Factor of the local point spacing");
    const Scalar in_smax   = opt.get_float("scale_max",   "smax"  ).set_default(1) .set_brief("Factor of the aabb diag length");
    const int    in_k      = opt.get_int(  "knn",         "k"     ).set_default(10).set_brief("Nearest neighbors count for the minimal scale");

    const Scalar in_alpha = opt.get_float("alpha", "a").set_default(0.1).set_brief("Sub-sampling factor for the multi-resolution");

    const Scalar in_mls_eps    = opt.get_float( "mls_eps"   ).set_default(0.01).set_brief("MLS convergence threshold (factor of the scale)");
    const int    in_mls_max    = opt.get_int(   "mls_max"   ).set_default(10)  .set_brief("MLS step max");
    const Scalar in_irls_sigma = opt.get_float( "irls_sigma").set_default(1.0) .set_brief("IRLS factor");
    const int    in_irls_step  = opt.get_int(   "irls_step" ).set_default(5)   .set_brief("IRLS step");

    const bool in_v = opt.get_bool("verbose", "v").set_default(false).set_brief("Add verbose messages");

    bool ok = opt.ok();
    if(!ok) return 1;

    PointCloud points;
    ok = Loader::Load(in_input, points, in_v);
    if(!ok) return 1;
    const int point_count = points.size();

    if(!points.has_normals())
    {
        error().iff(in_v) << "Normal vectors are required!";
        return 1;
    }

    points.build_kdtree();

    // 1. Scales ---------------------------------------------------------------
    info().iff(in_v) << "Computing " << in_scount << " scales";

    std::vector<Scalar> dist_k(point_count, 0);

    #pragma omp parallel for
    for(int i=0; i<point_count; ++i)
    {
        const Scalar squared_dist = points.kdtree().k_nearest_neighbors(i, in_k).search().bottom().squared_distance;
        dist_k[i] = std::sqrt(squared_dist);
    }
    std::sort(dist_k.begin(), dist_k.end());
    const Scalar local_point_spacing = dist_k[0.50*(point_count-1)]; // median
    const Scalar aabb_diag = points.aabb_diag();

    const int    scale_count = in_scount;
    const Scalar scale_min   = in_smin * local_point_spacing;
    const Scalar scale_max   = in_smax * aabb_diag;

    ScaleSampling scales;
    scales.log_sample(scale_min, scale_max, scale_count);

    info().iff(in_v) << "  min   = " << scale_min;
    info().iff(in_v) << "  max   = " << scale_max;

    // 2. Features -------------------------------------------------------------
    info().iff(in_v) << "Computing features";

    MultiScaleFeatures features(point_count, scale_count);

    // sampling stuff
    std::vector<bool> is_free(point_count);
    std::vector<int> sampling(point_count);
    std::vector<int> sampling2(point_count);
    std::vector<int> rank(point_count);

    // start with the full indices
    std::iota(sampling.begin(), sampling.end(), 0);

    // for each scale
    for(int j=0; j<scale_count; ++j)
    {
        info().iff(in_v) << j+1 << "/" << scale_count;

        const Scalar scale  = scales[j];

        // 2.1 poisson disk sampling -------------------------------------------
        {
            if(j == 0)
            {
                std::iota(sampling.begin(), sampling.end(), 0);
                // kdtree already built
            }
            else
            {
                rank.resize(sampling.size());
                std::iota(rank.begin(), rank.end(), 0);
                std::random_shuffle(rank.begin(), rank.end());
                std::fill(is_free.begin(), is_free.end(), true);

                sampling2.clear();

                const Scalar radius = in_alpha * scale;

#if 1
                // first version (less efficient because assume heterogeneous scale sampling)
                for(int idx_sample : rank)
                {
                    const int idx_point = sampling[idx_sample];
                    bool free = true;
                    for(int idx_nei : points.kdtree().range_neighbors(idx_point, radius))
                    {
                        if(!is_free[idx_nei])
                        {
                            free = false;
                            break;
                        }
                    }

                    if(free)
                    {
                        sampling2.emplace_back(idx_point);
                        is_free[idx_point] = false;

                        for(int idx_nei : points.kdtree().range_neighbors(idx_point, radius)) is_free[idx_nei] = false;
                    }
                }
#else
                // second version (more efficient because assume homogeneous scale sampling)
                // (these two versions do not give the same results)
                for(int idx_sample : rank)
                {
                    const int idx_point = sampling[idx_sample];
                    if(is_free[idx_point])
                    {
                        sampling2.emplace_back(idx_point);
                        is_free[idx_point] = false;

                        for(int idx_nei : points.kdtree().range_neighbors(idx_point, 2*radius)) // factor 2 !!!!
                            is_free[idx_nei] = false;
                    }
                }
#endif
                std::swap(sampling, sampling2);
                points.kdtree().rebuild(sampling);
            }
//            debug() << sampling.size() << " points sampled";
        }

        // 2.2 Features --------------------------------------------------------
        {
            RIMLSOperator mls;
            mls.set_scale(scale);
            mls.set_step_max(in_mls_max);
            mls.set_convergence_ratio_min(in_mls_eps);
            mls.set_reweighting_step(in_irls_step);
            mls.set_reweighting_sigma(in_irls_sigma);

            #pragma omp parallel for firstprivate(mls)
            for(int i=0; i<point_count; ++i)
            {
                Vector3 p = points[i];
                mls.compute(points, p);

                if(mls.stable())
                {
                    features.normal(i,j) = mls.fit().normal();
                    features.k1(i,j) = mls.fit().k1() * scale; // normalized curvature
                    features.k2(i,j) = mls.fit().k2() * scale;
                }
                else
                {
                    features.normal(i,j) = Vector3::Zero();
                    features.k1(i,j) = 0;
                    features.k2(i,j) = 0;
                }
            } // for i
        }
    } // for j

    features.save(in_output + "_features.txt");
    scales.save(  in_output + "_scales.txt");

    return 0;
}
