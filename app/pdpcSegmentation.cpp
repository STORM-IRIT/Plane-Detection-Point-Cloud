#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>
#include <PDPC/Segmentation/SeededKNNGraphRegionGrowing.h>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input    = opt.get_string("input",    "i").set_brief("Input point cloud (.ply/.obj)").set_required();
    const std::string in_features = opt.get_string("features", "f").set_brief("Input features (.txt/.bin)").set_required();

    const int    in_k     = opt.get_int(  "knn",  "k").set_default(10).set_brief("Region growing nearest neighbors count");
    const Scalar in_theta = opt.get_float("theta"    ).set_default(5 ).set_brief("Region growing angular threshold (degrees)");

    const bool in_v = opt.get_bool("verbose", "v").set_default(false).set_brief("Add verbose messages");

    bool ok = opt.ok();
    if(!ok) return 1;

    PointCloud points;
    ok = Loader::Load(in_input, points, in_v);
    if(!ok) return 1;
    const int point_count = points.size();

    MultiScaleFeatures features;
    ok = features.load(in_features, in_v);
    if(!ok) return 1;
    const int scale_count = features.m_scale_count;

    if(features.m_point_count != point_count)
    {
        error().iff(in_v) << "Point counts do not match: " << features.m_point_count << " != " << point_count;
        return 1;
    }


    // 1. Segmentations --------------------------------------------------------
    info().iff(in_v) << "Performing " << scale_count << " planar region growing";

    points.build_knn_graph(in_k);

    for(int j=0; j<scale_count; ++j)
    {
        SeededKNNGraphRegionGrowing::compute(points, seg,
        [](){},
        [](){},
        [](){});
    }

    return 0;
}
