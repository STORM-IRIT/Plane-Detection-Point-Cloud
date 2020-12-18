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
        std::ofstream ofs(filename);
        if(!ofs.is_open())
        {
            error().iff(v) << "Failed to open output features file " << filename;
            return false;
        }

        ofs << m_point_count << " " << m_scale_count << "\n";
        for(int i=0; i<m_point_count; ++i)
        {
            for(int j=0; j<m_scale_count; ++j)
            {
                ofs << normal(i,j)[0] << " "
                    << normal(i,j)[1] << " "
                    << normal(i,j)[2] << " "
                    << k1(i,j)        << " "
                    << k2(i,j)        << " ";
            }
            ofs << "\n";
        }
        return true;
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
        std::ifstream ifs(filename);
        if(!ifs.is_open())
        {
            error().iff(v) << "Failed to open input features file " << filename;
            return false;
        }

        ifs >> m_point_count;
        ifs >> m_scale_count;

        this->resize(m_point_count, m_scale_count);

        for(int i=0; i<m_point_count; ++i)
        {
            for(int j=0; j<m_scale_count; ++j)
            {
                ifs >> normal(i,j)[0];
                ifs >> normal(i,j)[1];
                ifs >> normal(i,j)[2];
                ifs >> k1(i,j);
                ifs >> k2(i,j);
            }
        }
        return true;
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
