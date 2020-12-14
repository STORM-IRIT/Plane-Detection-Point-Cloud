#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>
#include <PDPC/Common/Log.h>

#include <fstream>

namespace pdpc {

MultiScaleFeatures::MultiScaleFeatures(int point_count, int scale_count) :
    m_point_count(point_count),
    m_scale_count(scale_count),
    m_normals(point_count * scale_count),
    m_curvatures(point_count * scale_count)
{
}

bool MultiScaleFeatures::save(const std::string& filename, bool v) const
{
    const std::string ext = filename.substr(filename.find_last_of(".") + 1);
    if(ext == "txt")
    {
        PDPC_TODO; // txt not yet implemented
        return false;
    }
    else
    {
        const std::string filename_bin = (ext == "bin") ? (filename) : (filename + ".bin");
        std::ofstream ofs(filename_bin);
        if(!ofs.is_open())
        {
            error().iff(v) << "Failed to open output features file " << filename_bin;
            return false;
        }

        ofs.write(reinterpret_cast<const char*>(&m_point_count), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(&m_scale_count), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(m_normals.data()),    3 * sizeof(Scalar) * m_normals.size());
        ofs.write(reinterpret_cast<const char*>(m_curvatures.data()), 2 * sizeof(Scalar) * m_curvatures.size());

        info().iff(v) << m_point_count << "x" << m_scale_count
                      << " features saved to " << filename_bin;
    }

    return true;
}

bool MultiScaleFeatures::load(const std::string& filename, bool v)
{
    this->clear();

    const std::string ext = filename.substr(filename.find_last_of(".") + 1);
    if(ext == "txt")
    {
        PDPC_TODO; // txt not yet implemented
        return false;
    }
    else if(ext == "bin")
    {
        std::ifstream ifs(filename);
        if(!ifs.is_open())
        {
            error().iff(v) << "Failed to open input features file " << filename;
            return false;
        }

        ifs.read(reinterpret_cast<char*>(&m_point_count), sizeof(int));
        ifs.read(reinterpret_cast<char*>(&m_scale_count), sizeof(int));

        this->resize(m_point_count, m_scale_count);

        ifs.read(reinterpret_cast<char*>(m_normals.data()),    3 * sizeof(Scalar) * m_normals.size());
        ifs.read(reinterpret_cast<char*>(m_curvatures.data()), 2 * sizeof(Scalar) * m_curvatures.size());

        info().iff(v) << m_point_count << "x" << m_scale_count
                      << " features loaded from " << filename;
    }
    else
    {
        error().iff(v) << "Unkown extension " << ext << " for features file";
        return false;
    }
    return true;
}

void MultiScaleFeatures::clear()
{
    m_point_count = 0;
    m_scale_count = 0;
    m_normals.clear();
    m_curvatures.clear();
}

void MultiScaleFeatures::resize(int point_count, int scale_count)
{
    m_point_count = point_count;
    m_scale_count = scale_count;
    m_normals.resize(point_count * scale_count);
    m_curvatures.resize(point_count * scale_count);
}

} // namespace pdpc
