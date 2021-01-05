#include <PDPC/Persistence/ComponentDataSet.h>
#include <PDPC/Persistence/ComponentSet.h>
#include <PDPC/Segmentation/RegionSet.h>

#include <fstream>

namespace pdpc {

ComponentDataSet::ComponentDataSet(const ComponentSet& comp_set, RegionSet&& reg_set) :
    m_data(comp_set.size())
{
    for(int i=0; i<comp_set.size(); ++i)
    {
        m_data[i].m_birth   = comp_set[i].birth_level();
        m_data[i].m_death   = comp_set[i].death_level();
        m_data[i].m_indices = std::move(reg_set[i]);
    }
}

bool ComponentDataSet::save(const std::string& filename) const
{
    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        warning() << "Failed to open output file " << filename;
        return false;
    }

    for(int i=0; i<this->size(); ++i)
    {
        const auto& comp = m_data[i];

        ofs << comp.birth_level()
            << " "
            << comp.death_level()
            << " ";

        for(int idx : comp.indices())
            ofs << idx << " ";

        ofs << "\n";
    }

    return true;
}

bool ComponentDataSet::load(const std::string& filename)
{
    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        warning() << "Failed to open input file " << filename;
        return false;
    }

    std::string line;
    while(std::getline(ifs, line))
    {
        m_data.push_back(ComponentData());
        auto& comp = m_data.back();

        std::stringstream str(line);

        str >> comp.m_birth;
        str >> comp.m_death;

        int i;
        while(str >> i) comp.indices().push_back(i);
    }

    return true;
}

} // namespace pdpc
