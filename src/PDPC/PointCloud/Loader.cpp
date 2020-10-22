#include <PDPC/PointCloud/Loader.h>
#include <PDPC/PointCloud/OBJ.h>
#include <PDPC/PointCloud/PLY.h>

#include <PDPC/Common/Log.h>

namespace pdpc {

bool Loader::Load(const std::string& filename, PointCloud& g, bool verbose)
{
    const std::string ext = filename.substr(filename.find_last_of(".") + 1);
    if(ext == "obj")
    {
        return OBJ::load(filename, g, verbose);
    }
    else if(ext == "ply")
    {
        return PLY::load(filename, g, verbose);
    }
    else
    {
        error() << "Missing or unsupported extension for file " << filename;
        return false;
    }
}

bool Loader::Save(const std::string& filename, const PointCloud& g, bool verbose)
{
    const std::string ext = filename.substr(filename.find_last_of(".") + 1);
    if(ext == "obj")
    {
        return OBJ::save(filename, g, verbose);
    }
    else if(ext == "ply")
    {
        return PLY::save(filename, g, verbose);
    }
    else
    {
        return PLY::save(filename + ".ply", g, verbose);
    }
}

} // namespace pdpc
