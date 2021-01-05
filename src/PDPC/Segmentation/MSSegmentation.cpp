#include <PDPC/Segmentation/MSSegmentation.h>
#include <PDPC/Common/Log.h>

#include <fstream>

namespace pdpc {

// Modifiers -------------------------------------------------------------------

void MSSegmentation::make_continuous()
{
    for_each([](auto& seg){seg.make_continuous();});
}

void MSSegmentation::make_valid(int l)
{
    for_each([l](auto& seg){seg.make_valid(l);});
}

bool MSSegmentation::is_valid() const
{
    for(const Segmentation& seg : m_data)
    {
        if(!seg.is_valid())
            return false;
    }
    return true;
}

bool MSSegmentation::load(const std::string& filename)
{
    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        warning() << "Failed to open output file " << filename;
        return false;
    }

    int point_count = 0;
    int scale_count = 0;

    ifs >> point_count;
    ifs >> scale_count;

    PDPC_DEBUG_ASSERT(point_count > 0);
    PDPC_DEBUG_ASSERT(scale_count > 0);

    this->resize(scale_count);

    std::vector<std::vector<int>> labels(scale_count, std::vector<int>(point_count, -1));

    for(int i=0; i<point_count; ++i)
    {
        for(int j=0; j<scale_count; ++j)
        {
            ifs >> labels[j][i];
        }
    }
    for(int j=0; j<scale_count; ++j)
    {
        m_data[j] = Segmentation(labels[j]);
    }
    return true;
}

bool MSSegmentation::save(const std::string& filename) const
{
    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        warning() << "Failed to open output file " << filename;
        return false;
    }

    PDPC_ASSERT(!m_data.empty());
    const int point_count = m_data.at(0).size();
    const int scale_count = m_data.size();

    ofs << point_count << " " << scale_count << "\n";

    for(int i=0; i<point_count; ++i)
    {
        for(int j=0; j<scale_count; ++j)
        {
            ofs << m_data[j][i] << " ";
        }
        ofs << "\n";
    }

    return true;
}

} // namespace pdpc
