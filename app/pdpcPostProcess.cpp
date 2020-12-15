#include <PDPC/Common/Option.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Segmentation/MSSegmentation.h>
#include <PDPC/Segmentation/RegionSet.h>
#include <PDPC/Persistence/ComponentSet.h>

#include <fstream>

using namespace pdpc;

int main(int argc, char **argv)
{
    Option opt(argc, argv);
    const std::string in_seg  = opt.get_string("seg",  "s").set_brief("TODO").set_default("output_seg.bin");
    const std::string in_reg  = opt.get_string("reg",  "r").set_brief("TODO").set_default("output_reg.bin");
    const std::string in_comp = opt.get_string("comp", "c").set_brief("TODO").set_default("output_comp.bin");

    const bool in_v = opt.get_bool("verbose", "v").set_default(false).set_brief("Add verbose messages");

    bool ok = opt.ok();
    if(!ok) return 1;

    MSSegmentation comp_seg;
    std::ifstream ifs_seg(in_seg);
    comp_seg.read(ifs_seg);
    ifs_seg.close();

    RegionSet reg_set;
    std::ifstream ifs_reg(in_reg);
    reg_set.read(ifs_reg);
    ifs_reg.close();

    ComponentSet comp_set;
    std::ifstream ifs_comp(in_comp);
    comp_set.read(ifs_comp);
    ifs_comp.close();



    return 0;
}
