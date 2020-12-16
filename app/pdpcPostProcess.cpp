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
    const std::string in_input  = opt.get_string("input",  "i").set_brief("Input point cloud (.ply/.obj)").set_required();
    const std::string in_seg    = opt.get_string("seg",    "s").set_brief("TODO").set_default("output_seg.bin");
    const std::string in_reg    = opt.get_string("reg",    "r").set_brief("TODO").set_default("output_reg.bin");
    const std::string in_comp   = opt.get_string("comp",   "c").set_brief("TODO").set_default("output_comp.bin");
    const std::string in_output = opt.get_string("output", "o").set_brief("Output name").set_default("output");

    const std::vector<std::string> in_ranges = opt.get_strings("range" ).set_brief("Persistence ranges");
    const std::vector<std::string> in_scales = opt.get_strings("scales").set_brief("Scales threshold");

    const bool in_col = opt.get_bool("colorize", "col").set_default(false).set_brief("Colorize point cloud and save it as ply");

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

//    constexpr int scale_sup = 1000; //?
//    const int scale_count_sup = std::min(scale_count, scale_sup);

    // info
    info() << comp_set.size() << " components";
    for(int i=0; i<comp_set.size(); ++i)
    {
        info() << i << ": " << comp_set[i].birth_level() << " --- "
               << comp_set[i].death_level() << " = "
               << comp_set[i].persistence()
               << " (" << reg_set[i].size() << " pts)";
    }

    // Persistence range -------------------------------------------------------
    if(!in_ranges.empty())
    {
        if(in_ranges.size() % 2 == 0)
        {
            for(int n=0; n<int(in_ranges.size())/2; ++n)
            {
                const int pers_min = std::stoi(in_ranges[2*n + 0]);
                const int pers_max = std::stoi(in_ranges[2*n + 1]);
                info().iff(in_v) << "Extracting component with persistence in "
                                 << "(" << pers_min << "," << pers_max << ")";
                if(pers_max < pers_min) warning().iff(in_v) << "Persistence range must be ordered, the output will be empty";

                Segmentation res(point_count);
                res.resize_region(comp_set.size());
                for(int c=0; c<comp_set.size(); ++c)
                {
                    if(pers_min <= comp_set[c].persistence() && comp_set[c].persistence() <= pers_max)
                    {
                        for(int i : reg_set[c]) res.set_label(i, c);
                    }
                }

                if(in_col)
                {
                    points.request_colors();
                    const auto colormap = Colormap::Tab20();
                    res.set_colors(points.colors_data(), Colors::Black(), colormap);
                    Loader::Save(in_output + "_range_" + str::to_string(n,3) + ".ply", points);
                }
                debug() << "TODO save txt file";
            }
        }
        else
        {
            info().iff(in_v) << "Persistence ranges input must contains an even number of persistence value";
        }
    }

    // Scale -------------------------------------------------------------------
    if(!in_scales.empty())
    {
//        std::vector<int> labeling(point_count, -1);
//        MSSegmentation ms_seg(scale_count);

        for(std::string str : in_scales)
        {
            const int idx_scale = std::stoi(str);

            std::vector<int> labeling(point_count, -1);

            #pragma omp parallel for
            for(int idx_point=0; idx_point<point_count; ++idx_point)
            {
                int max_persistence = 0;
                int max_idx_comp    = -1;

                for(int j=0; j<scale_count; ++j)
                {
                    const int idx_comp = comp_seg[j][idx_point];
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
            Segmentation seg(labeling);
            seg.invalidate_small_region(10);

            // save
            if(in_col)
            {
                points.request_colors();
                const auto colormap = Colormap::Tab20();
                seg.set_colors(points.colors_data(), Colors::Black(), colormap);
                Loader::Save(in_output + "_range_" + str::to_string(idx_scale,3) + ".ply", points);
            }
            debug() << "TODO save txt file";
        }
    }

//    // -------------------------------------------------------------------------
//    // 2. Show components by persistence (plugin Action)
//    {
//        points.request_colors(Colors::Black());
//        const auto colormap = Colormap::Tab20();
//        for(int pers=0; pers<scale_count; ++pers)
//        {
//            for(int idx_comp=0; idx_comp<comp_set.size(); ++idx_comp)
//            {
//                if(comp_set[idx_comp].persistence() >= pers)
//                {
//                    for(int i : reg_set[idx_comp])
//                        points.color(i) = colormap[idx_comp];
//                }
//                else
//                {
//                    // comp are sorted by pers
//                    break;
//                }
//            }
//            Loader::Save("test2_" + str::to_string(pers,2)+".ply", points);
//        }
//    }





    return 0;
}
