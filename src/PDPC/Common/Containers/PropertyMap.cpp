#include <PDPC/Common/Containers/PropertyMap.h>

namespace pdpc {


// Constructor -----------------------------------------------------------------

property_map::property_map() :
    poly_vector(),
    m_mapID(),
    m_vecID()
{
}

property_map::property_map(const property_map& other) :
    poly_vector(other),
    m_mapID(other.m_mapID),
    m_vecID(other.m_vecID)
{
}

property_map::property_map(property_map&& other) :
    poly_vector(other),
    m_mapID(std::move(other.m_mapID)),
    m_vecID(std::move(other.m_vecID))
{
}

property_map::property_map(int size) :
    poly_vector(size),
    m_mapID(),
    m_vecID()
{
}

property_map::~property_map()
{
}

bool property_map::match(const property_map& other) const
{
    return m_mapID == other.m_mapID &&
           m_vecID == other.m_vecID &&
           poly_vector::match(other);
}

void property_map::transfer_from(const property_map& other)
{
    poly_vector::transfer_from(other);
    m_mapID = other.m_mapID;
    m_vecID = other.m_vecID;
}

property_map& property_map::operator = (const property_map& other)
{
    poly_vector::operator=(other);
    m_mapID = other.m_mapID;
    m_vecID = other.m_vecID;
    return *this;
}

property_map& property_map::operator = (property_map&& other)
{
    poly_vector::operator=(other);
    m_mapID = std::move(other.m_mapID);
    m_vecID = std::move(other.m_vecID);
    return *this;
}

void property_map::clean()
{
    this->discard_all();
}

// IO --------------------------------------------------------------------------

std::ostream& property_map::write(std::ostream& os) const
{
    poly_vector::write(os);
    for(int idx=0; idx<int(m_vecID.size()); ++idx)
    {
        IO::write_value(os, m_vecID[idx]);
    }
    return os;
}

std::istream& property_map::read(std::istream& is)
{
    this->discard_all();

    poly_vector::read(is);
    m_vecID.resize(this->count());
    for(int idx=0; idx<int(m_vecID.size()); ++idx)
    {
        IO::read_value(is, m_vecID[idx]);
        m_mapID[m_vecID[idx]] = idx;
    }
    return is;
}

// Add/Remove ------------------------------------------------------------------

int property_map::add(const String& name, IO::TypeID type)
{
    int idx = poly_vector::add(type);

    if(int(m_vecID.size()) < idx+1)
        m_vecID.resize(idx+1);

    m_vecID[idx]  = name;
    m_mapID[name] = idx;

    return idx;
}

void property_map::discard(const String& name)
{
    auto it = m_mapID.find(name);
    if(it != m_mapID.end())
    {
        poly_vector::discard(it->second);
    }
}

void property_map::discard_all()
{
    poly_vector::discard_all();
    m_mapID.clear();
    m_vecID.clear();
}

// Id Accessors ----------------------------------------------------------------

int property_map::index(const String& name) const
{
    auto it = m_mapID.find(name);
    return it->second;
}

property_map::String property_map::name(int idx) const
{
    return m_vecID[idx];
}

void property_map::rename(const String& from, const String& to)
{
    auto it = m_mapID.find(from);

    if(it != m_mapID.end())
    {
        int idx = it->second;

        m_mapID.erase(it);

        m_vecID[idx] = to;
        m_mapID[to]  = idx;
    }
}

// Checking --------------------------------------------------------------------

bool property_map::has(const String& name) const
{
    auto it = m_mapID.find(name);
    return it != m_mapID.end() && poly_vector::has(it->second);
}

bool property_map::_is_valid() const
{
    if(m_mapID.size() != m_vecID.size())
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    if(m_mapID.size() != poly_vector::m_vectors.size())
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    for(int i=0; i<int(m_vecID.size()); ++i)
    {
        auto it = m_mapID.find(m_vecID[i]);
        if(it == m_mapID.end())
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
        if(it->second != i)
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    for(const auto& it : m_mapID)
    {
        if(it.second >= int(m_vecID.size()))
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
        if(m_vecID[it.second] != it.first)
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    return poly_vector::_is_valid();
}

// Internal --------------------------------------------------------------------

const abstract_vector& property_map::get(const String& name) const
{
    return *m_vectors[index(name)].get();
}

abstract_vector& property_map::get(const String& name)
{
    return *m_vectors[index(name)].get();
}

} // namespace pdpc
