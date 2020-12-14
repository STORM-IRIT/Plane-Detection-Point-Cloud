#pragma once

#include <PDPC/Common/Containers/PropertyMap.h>
#include <PDPC/Common/Assert.h>

#include <vector>

namespace pdpc {

class RegionSet
{
    // Types -------------------------------------------------------------------
public:
    using Region      = std::vector<int>;
    using RegionArray = std::vector<Region>;

    // RegionSet ---------------------------------------------------------------
public:
    RegionSet();
    RegionSet(int region_count);

    // IO ----------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);

    // Capacity ----------------------------------------------------------------
public:
    inline int size() const;
    inline int size(int i) const;
    inline bool empty() const;
    inline bool empty(int i) const;

    // Accessors ---------------------------------------------------------------
public:
    inline int  index(int i, int j) const;
    inline int& index(int i, int j);

    inline const Region& region(int i) const;
    inline       Region& region(int i);

    inline const Region& operator[](int i) const;
    inline       Region& operator[](int i);

    inline const RegionArray& regions() const;
    inline       RegionArray& regions();

    // Modifiers ---------------------------------------------------------------
public:
    void clear();
    void clear(int i);
    void resize(int region_count);
    void resize(int i, int count);

    // Properties --------------------------------------------------------------
public:
    template<typename P>
    inline int add(const std::string& name);

    template<typename P>
    inline void discard(const std::string& name);

    template<typename P>
    inline void discard(int prop);

    template<typename P>
    inline const P& property(int prop, int i) const;

    template<typename P>
    inline P& property(int prop, int i);

    template<typename P>
    inline const P& property(const std::string& name, int i) const;

    template<typename P>
    inline P& property(const std::string& name, int i);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& property(int prop) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& property(int prop);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& property(const std::string& name) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& property(const std::string& name);

    inline const property_map& properties() const;
    inline       property_map& properties();

protected:
    RegionArray  m_regions;
    property_map m_properties;

}; // class RegionSet

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Capacity --------------------------------------------------------------------

int RegionSet::size() const
{
    return regions().size();
}

int RegionSet::size(int i) const
{
    return region(i).size();
}

bool RegionSet::empty() const
{
    return regions().empty();
}

bool RegionSet::empty(int i) const
{
    return region(i).empty();
}

// Accessors -------------------------------------------------------------------

int  RegionSet::index(int i, int j) const
{
    PDPC_DEBUG_ASSERT(0 <= j && j < int(region(i).size()));
    return region(i)[j];
}

int& RegionSet::index(int i, int j)
{
    PDPC_DEBUG_ASSERT(0 <= j && j < int(region(i).size()));
    return region(i)[j];
}

const RegionSet::Region& RegionSet::region(int i) const
{
    PDPC_DEBUG_ASSERT(0 <= i && i < int(regions().size()));
    return regions()[i];
}

RegionSet::Region& RegionSet::region(int i)
{
    PDPC_DEBUG_ASSERT(0 <= i && i < int(regions().size()));
    return regions()[i];
}

const RegionSet::Region& RegionSet::operator[](int i) const
{
    return region(i);
}

RegionSet::Region& RegionSet::operator[](int i)
{
    return region(i);
}

const RegionSet::RegionArray& RegionSet::regions() const
{
    return m_regions;
}

RegionSet::RegionArray& RegionSet::regions()
{
    return m_regions;
}

// Properties ------------------------------------------------------------------

template<typename P>
int RegionSet::add(const std::string& name)
{
    return properties().add<P>(name);
}

template<typename P>
void RegionSet::discard(const std::string& name)
{
    return properties().discard<P>(name);
}

template<typename P>
void RegionSet::discard(int prop)
{
    return properties().discard<P>(prop);
}

template<typename P>
const P& RegionSet::property(int prop, int i) const
{
    return properties().at<P>(prop, i);
}

template<typename P>
P& RegionSet::property(int prop, int i)
{
    return properties().at<P>(prop, i);
}

template<typename P>
const P& RegionSet::property(const std::string& name, int i) const
{
    return properties().at<P>(name, i);
}

template<typename P>
P& RegionSet::property(const std::string& name, int i)
{
    return properties().at<P>(name, i);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& RegionSet::property(int prop) const
{
    return properties().at<P>(prop);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& RegionSet::property(int prop)
{
    return properties().at<P>(prop);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& RegionSet::property(const std::string& name) const
{
    return properties().at<P>(name);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& RegionSet::property(const std::string& name)
{
    return properties().at<P>(name);
}

const property_map& RegionSet::properties() const
{
    return m_properties;
}

property_map& RegionSet::properties()
{
    return m_properties;
}

} // namespace pdpc

