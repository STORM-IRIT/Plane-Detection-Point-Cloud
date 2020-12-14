#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/PointCloud/orthonormal_basis.h>
#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>
#include <PDPC/ScaleSpace/ScaleSampling.h>
#include <PDPC/Segmentation/SeededKNNGraphRegionGrowing.h>
#include <PDPC/Segmentation/MSSegmentation.h>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input    = opt.get_string("input",    "i").set_brief("Input point cloud (.ply/.obj)").set_required();
    const std::string in_scales   = opt.get_string("scales",   "s").set_brief("Input scales (.txt)").set_required();
    const std::string in_features = opt.get_string("features", "f").set_brief("Input features (.txt/.bin)").set_required();

    const int    in_k     = opt.get_int(  "knn",  "k").set_default(10).set_brief("Region growing nearest neighbors count");
    const Scalar in_theta = opt.get_float("theta"    ).set_default(5.).set_brief("Region growing angular threshold (degrees)");
    const Scalar in_phi   = opt.get_float("phi"      ).set_default(1.).set_brief("Region growing curvature threshold");

    const bool in_v = opt.get_bool("verbose", "v").set_default(false).set_brief("Add verbose messages");

    bool ok = opt.ok();
    if(!ok) return 1;

    PointCloud points;
    ok = Loader::Load(in_input, points, in_v);
    if(!ok) return 1;
    const int point_count = points.size();

    ScaleSampling scales;
    ok = scales.load(in_scales, in_v);
    const int scale_count = scales.size();

    MultiScaleFeatures features;
    ok = features.load(in_features, in_v);
    if(!ok) return 1;

    if(features.m_point_count != point_count)
    {
        error().iff(in_v) << "Point counts do not match: " << features.m_point_count << " != " << point_count;
        return 1;
    }
    if(features.m_scale_count != scale_count)
    {
        error().iff(in_v) << "Scale counts do not match: " << features.m_scale_count << " != " << scale_count;
        return 1;
    }

    // 1. Segmentations --------------------------------------------------------
    info().iff(in_v) << "Performing " << scale_count << " planar region growing";

    MSSegmentation ms_seg(scale_count, Segmentation(point_count));
    const Scalar threshold_angle = std::cos(in_theta / 180. * M_PI);
    const Scalar threshold_curva = in_phi;

    points.build_knn_graph(in_k);

    #pragma omp parallel for
    for(int j=0; j<scale_count; ++j)
    {
        #pragma omp critical (seg_info)
        {
            info().iff(in_v) << "Processing scale " << j << "/" << scale_count-1 << " (" << int(Scalar(j)/(scale_count-1)*100) << "%)...";
        }

        std::vector<int> seeds;
        Segmentation& seg = ms_seg[j];

        SeededKNNGraphRegionGrowing::compute(points, seg,
        // Comparison function for growing
        [&features,&seeds,j,threshold_angle,threshold_curva](int rg_l, int rg_i, int rg_j) -> bool
        {
            const int idx_seed = seeds[rg_l];
            PDPC_UNUSED(rg_i);
            return features.normal(idx_seed,j).dot(features.normal(rg_j,j)) > threshold_angle &&
                   features.plane_dev(rg_j,j) < threshold_curva;
        },
        // Priority function for seeds
        [&features,j](int rg_i, int rg_j) -> bool
        {
            return features.plane_dev(rg_i, j) > features.plane_dev(rg_j, j);
        },
        // Called for region initialization
        [&seeds](int rg_l, int rg_i)
        {
            PDPC_UNUSED(rg_l);
            seeds.push_back(rg_i);
        });

        PDPC_ASSERT(seg.region_count() == int(seeds.size()));

        // filtering
        std::vector<bool> to_invalidate(seg.region_count(), false);
        std::vector<std::vector<int>> regions;
        seg.fill(regions);

        for(int l=0; l<seg.region_count(); ++l)
        {
            const Vector3 plane_p = points.point(seeds[l]);
            const Vector3 plane_n = points.normal(seeds[l]);
            const Matrix3 plane_T = orthonormal_basis(plane_n);

            Aabb2 aabb;
            for(int i : regions[l])
            {
                aabb.extend( (plane_T * (points[i]-plane_p)).head<2>() );
            }
            to_invalidate[l] = aabb.diagonal().norm() < scales[j];
        }
        seg.invalidate(to_invalidate);
        seg.make_full();
    }



    return 0;
}
