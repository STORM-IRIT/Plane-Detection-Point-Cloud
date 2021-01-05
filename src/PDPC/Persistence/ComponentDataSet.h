#pragma once

#include <vector>
#include <string>

namespace pdpc {

class ComponentSet;
class RegionSet;

class ComponentData
{
public:
    inline ComponentData() = default;
    inline ComponentData(int birth, int death) : m_birth(birth), m_death(death), m_indices() {}

    inline int birth_level() const {return m_birth;}
    inline int death_level() const {return m_death;}
    inline int persistence() const {return m_death - m_birth;}

    inline int size() const {return m_indices.size();}

    inline const std::vector<int>& indices() const {return m_indices;}
    inline       std::vector<int>& indices()       {return m_indices;}

public:
    int m_birth;
    int m_death;
    std::vector<int> m_indices;
};

// =============================================================================
// =============================================================================
// =============================================================================

class ComponentDataSet
{
public:
    ComponentDataSet() = default;
    ComponentDataSet(const ComponentSet& comp_set, RegionSet&& reg_set);

    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    inline int size() const {return m_data.size();}

    inline const ComponentData& operator[](int i) const {return m_data[i];}
    inline       ComponentData& operator[](int i)       {return m_data[i];}

public:
    std::vector<ComponentData> m_data;
};

} // namespace pdpc
