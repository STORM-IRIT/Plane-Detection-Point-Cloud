#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/SpacePartitioning/KdTree.h>
#include <PDPC/ScaleSpace/ScaleSampling.h>
#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>

#include <numeric>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input  = opt.get_string("input",       "i"     ).set_required()    .set_brief("Input point cloud (.ply or .obj)");
    const std::string in_output = opt.get_string("output",      "o"     ).set_default("features.bin").set_brief("Output features (.bin or .txt)");
    const Scalar      in_scount = opt.get_float( "scale_count", "scount").set_default(50)   .set_brief("Scale count");
    const Scalar      in_smin   = opt.get_float( "scale_min",   "smin"  ).set_default(1)    .set_brief("Factor of the local point spacing");
    const Scalar      in_smax   = opt.get_float( "scale_max",   "smax"  ).set_default(1)    .set_brief("Factor of the aabb diag length");
    const int         in_k      = opt.get_int(   "knn",         "k"     ).set_default(10)   .set_brief("Nearest neighbors count for the minimal scale");
    const Scalar      in_alpha  = opt.get_float( "alpha",       "a"     ).set_default(0.1)  .set_brief("Sub-sampling factor for the multi-resolution");
    const bool        in_v      = opt.get_bool(  "verbose",     "v"     ).set_default(false).set_brief("Add verbose messages");

    constexpr bool in_full    = false;
    constexpr bool in_uniform = true;

    bool ok = opt.ok();
    if(!ok) return 1;

    PointCloud points;
    ok = Loader::Load(in_input, points, in_v);
    if(!ok) return 1;
    const int point_count = points.size();

    if(!points.has_normals())
    {
        error().iff(in_v) << "Normal vectors are required! (use script pdpcComputeNormals)";
        return 1;
    }

    // 1. Scales ---------------------------------------------------------------
    info().iff(in_v) << "Computing " << in_scount << " scales";

    Scalar sum_dist = 0;

    #pragma omp parallel for
    for(int i=0; i<point_count; ++i)
    {
        const Scalar squared_dist = points.kdtree().k_nearest_neighbors(i, in_k).search().bottom().squared_distance;
        const Scalar dist = std::sqrt(squared_dist);

        #pragma omp atomic
        sum_dist += dist;
    }
    const Scalar local_point_spacing = sum_dist / point_count;
    const Scalar aabb_diag = points.aabb_diag();

    const int    scale_count = in_scount;
    const Scalar scale_min   = in_smin * local_point_spacing;
    const Scalar scale_max   = in_smax * aabb_diag;

    ScaleSampling scales;
    scales.log_sample(scale_min, scale_max, scale_count);

    info().iff(in_v) << "Scale count = " << scale_count;
    info().iff(in_v) << "Scale min   = " << scale_min;
    info().iff(in_v) << "Scale max   = " << scale_max;

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
        info().iff(j) << "Processing scale " << j << "/" << scale_count-1 << " (" << int(Scalar(j)/(scale_count-1)*100) << "%)...";

        // 2.1 poisson disk sampling -------------------------------------------
        if(j == 0 && !in_full)
        {
            // kdtree already built = no sub-sampling
        }
        else
        {
            rank.resize(sampling.size());
            std::iota(rank.begin(), rank.end(), 0);
            std::random_shuffle(rank.begin(), rank.end());
            std::fill(is_free.begin(), is_free.end(), true);

            sampling2.clear();

            const Scalar scale  = scales[j];
            const Scalar radius = in_alpha * scale;

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
            std::swap(sampling, sampling2);
            points.kdtree().rebuild(sampling);
        }

        // 2.2 Features --------------------------------------------------------
        #pragma omp parallel for
        for(int i=0; i<point_count; ++i)
        {
            PDPC_TODO;

        } // for i
    } // for j

    features.save(in_output);

    return 0;
}
