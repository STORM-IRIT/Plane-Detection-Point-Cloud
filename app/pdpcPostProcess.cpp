#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Common/Colors.h>
#include <PDPC/Common/String.h>
#include <PDPC/Segmentation/MSSegmentation.h>
#include <PDPC/Segmentation/RegionSet.h>
#include <PDPC/Persistence/ComponentSet.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>

#include <fstream>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input = opt.get_string("input", "i").set_brief("Input point cloud (.ply/.obj)").set_required();
    const std::string in_seg   = opt.get_string("seg",   "s").set_brief("TODO").set_default("output_seg.bin");
    const std::string in_reg   = opt.get_string("reg",   "r").set_brief("TODO").set_default("output_reg.bin");
    const std::string in_comp  = opt.get_string("comp",  "c").set_brief("TODO").set_default("output_comp.bin");

    const bool in_v = opt.get_bool("verbose", "v").set_default(false).set_brief("Add verbose messages");

    bool ok = opt.ok();
    if(!ok) return 1;

    PointCloud points;
    ok = Loader::Load(in_input, points, in_v);
    if(!ok) return 1;
    const int point_count = points.size();

    MSSegmentation comp_seg;
    std::ifstream ifs_seg(in_seg);
    PDPC_ASSERT(ifs_seg.is_open());
    comp_seg.read(ifs_seg);
    ifs_seg.close();

    RegionSet reg_set;
    std::ifstream ifs_reg(in_reg);
    PDPC_ASSERT(ifs_reg.is_open());
    reg_set.read(ifs_reg);
    ifs_reg.close();

    ComponentSet comp_set;
    std::ifstream ifs_comp(in_comp);
    PDPC_ASSERT(ifs_comp.is_open());
    comp_set.read(ifs_comp);
    ifs_comp.close();

    const int scale_count = comp_seg.size();
    constexpr int scale_sup = 1000; //?

    const int scale_count_sup = std::min(scale_count, scale_sup);

    // info
    info() << comp_set.size() << " components";
    for(int i=0; i<comp_set.size(); ++i)
    {
        info() << i << ": " << comp_set[i].birth_level() << " --- "
               << comp_set[i].death_level() << " = "
               << comp_set[i].persistence()
               << " (" << reg_set[i].size() << " pts)";
    }

    // -------------------------------------------------------------------------
    // 1. BestComponentPerScale
    {
        std::vector<int> labeling(point_count, -1);
        MSSegmentation ms_seg(scale_count_sup);

        for(int idx_scale=0; idx_scale<scale_count_sup; ++idx_scale)
        {
            info() << "Processing scale " << idx_scale << "/" << scale_count_sup-1;

            std::fill(labeling.begin(), labeling.end(), -1);

            #pragma omp parallel for
            for(int idx_point=0; idx_point<point_count; ++idx_point)
            {
                int max_persistence = 0;
                int max_idx_comp = -1;

                for(int j=0; j<scale_count_sup; ++j)
                {
                    int idx_comp = comp_seg[j][idx_point];
                    if(idx_comp == -1) continue;

                    const Component& comp = comp_set[idx_comp];

                    if(comp.birth_level() <= idx_scale && idx_scale <= comp.death_level())
                    {
                        if(comp.persistence() > max_persistence)
                        {
                            max_persistence = comp.persistence();
                            max_idx_comp = idx_comp;
                        }
                    }
                }
                labeling[idx_point] = max_idx_comp;
            }
            ms_seg[idx_scale] = Segmentation(labeling);
            ms_seg[idx_scale].invalidate_small_region(10);
        }


        // save
        points.request_colors();
        const auto colormap = Colormap::Tab20();
        for(int idx_scale=0; idx_scale<scale_count_sup; ++idx_scale)
        {
            const Segmentation& seg = ms_seg[idx_scale];
            seg.set_colors(points.colors_data(), Colors::Black(), colormap);
            Loader::Save("test1_" + str::to_string(idx_scale,2)+".ply", points);
        }
    }

    // -------------------------------------------------------------------------
    // 2. Show components by persistence (plugin Action)
    {
        points.request_colors(Colors::Black());
        const auto colormap = Colormap::Tab20();
        for(int pers=0; pers<scale_count; ++pers)
        {
            for(int idx_comp=0; idx_comp<comp_set.size(); ++idx_comp)
            {
                if(comp_set[idx_comp].persistence() >= pers)
                {
                    for(int i : reg_set[idx_comp])
                        points.color(i) = colormap[idx_comp];
                }
                else
                {
                    // comp are sorted by pers
                    break;
                }
            }
            Loader::Save("test2_" + str::to_string(pers,2)+".ply", points);
        }
    }





    return 0;
}
