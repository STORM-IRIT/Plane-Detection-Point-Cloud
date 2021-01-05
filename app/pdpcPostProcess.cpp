#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Common/Colors.h>
#include <PDPC/Common/String.h>
#include <PDPC/Segmentation/MSSegmentation.h>
#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/PointCloud.h>
#include <PDPC/Persistence/ComponentDataSet.h>

#include <fstream>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_input  = opt.get_string("input",  "i").set_brief("Input point cloud (.ply/.obj)" ).set_required();
    const std::string in_seg    = opt.get_string("seg",    "s").set_brief("Input multi-scale segmentation").set_default("output_seg.txt");
    const std::string in_comp   = opt.get_string("comp",   "c").set_brief("Input components"              ).set_default("output_comp.txt");
    const std::string in_output = opt.get_string("output", "o").set_brief("Output name"                   ).set_default("output");

    const std::vector<std::string> in_ranges = opt.get_strings("range"              ).set_brief("Persistence ranges");
    const std::vector<std::string> in_scales = opt.get_strings("scales"             ).set_brief("Scale thresholds");
    const std::vector<std::string> in_pers   = opt.get_strings("persistence", "pers").set_brief("Persistence thresholds");

    const bool in_col   = opt.get_bool("colorize", "col").set_default(false).set_brief("Colorize the point cloud and save it as PLY");
    const bool in_debug = opt.get_bool("debug"          ).set_default(false).set_brief("Print some debug info");
    const bool in_v     = opt.get_bool("verbose", "v"   ).set_default(false).set_brief("Add verbose messages");

    bool ok = opt.ok();
    if(!ok) return 1;

    PointCloud points;
    ok = Loader::Load(in_input, points, in_v);
    if(!ok) return 1;
    const int point_count = points.size();

    MSSegmentation comp_seg;
    ok = comp_seg.load(in_seg);
    if(!ok) return 1;

    ComponentDataSet comp_data;
    ok = comp_data.load(in_comp);
    if(!ok) return 1;

    const int scale_count = comp_seg.size();

    // debug info
    if(in_debug)
    {
        debug() << comp_data.size() << " components";
        for(int i=0; i<comp_data.size(); ++i)
        {
            debug() << i << ": " << comp_data[i].birth_level() << " --- "
                    << comp_data[i].death_level() << " = "
                    << comp_data[i].persistence()
                    << " (" << comp_data[i].size() << " pts)";
        }
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
                res.resize_region(comp_data.size());
                for(int c=0; c<comp_data.size(); ++c)
                {
                    if(pers_min <= comp_data[c].persistence() && comp_data[c].persistence() <= pers_max)
                    {
                        for(int i : comp_data[c].indices()) res.set_label(i, c);
                    }
                }

                // save
                const std::string filename = in_output + "_" + str::to_string(pers_min,3) + "_" + str::to_string(pers_max,3);
                res.save(filename + ".txt");
                if(in_col)
                {
                    points.request_colors();
                    const auto colormap = Colormap::Tab20();
                    res.set_colors(points.colors_data(), Colors::Black(), colormap);
                    Loader::Save(filename + ".ply", points, in_v);
                }
            }
        }
        else
        {
            info().iff(in_v) << "Persistence ranges input must contains an even number of persistence value";
        }
    }

    // Persistence threshold ---------------------------------------------------
    if(!in_pers.empty())
    {
        for(const auto& str : in_pers)
        {
            std::vector<int> labeling(point_count, -1);
            const int pers = std::stoi(str);

            for(int c=0; c<comp_data.size(); ++c)
            {
                if(comp_data[c].persistence() >= pers)
                {
                    for(int i : comp_data[c].indices())
                        labeling[i] = c;
                }
                else
                {
                    // comp are sorted by pers
                    break;
                }
            }
            Segmentation seg(labeling);
            seg.invalidate_small_region(10);

            // save
            const std::string filename = in_output + "_" + str::to_string(pers,3);
            seg.save(filename + ".txt");
            if(in_col)
            {
                points.request_colors();
                const auto colormap = Colormap::Tab20();
                seg.set_colors(points.colors_data(), Colors::Black(), colormap);
                Loader::Save(filename + ".ply", points, in_v);
            }
        }
    }

    // Scale -------------------------------------------------------------------
    if(!in_scales.empty())
    {
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

                    const auto& comp = comp_data[idx_comp];

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
            const std::string filename = in_output + "_" + str::to_string(idx_scale,3);
            seg.save(filename + ".txt");
            if(in_col)
            {
                points.request_colors();
                const auto colormap = Colormap::Tab20();
                seg.set_colors(points.colors_data(), Colors::Black(), colormap);
                Loader::Save(filename + ".ply", points, in_v);
            }
        }
    }

    return 0;
}
