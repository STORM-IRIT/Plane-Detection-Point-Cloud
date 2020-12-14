#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>

#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input = opt.get_string("input",   "i").set_required()    .set_brief("Input point cloud (ply/obj)");
    const bool        in_v     = opt.get_bool(  "verbose", "v").set_default(false).set_brief("Add verbose messages");

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



    return 0;
}
