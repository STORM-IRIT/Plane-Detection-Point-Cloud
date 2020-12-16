#include <PDPC/Persistence/ComponentSet.h>

namespace pdpc {

// ComponentSet ----------------------------------------------------------------

ComponentSet::ComponentSet() :
    m_components(),
    m_properties()
{
}

ComponentSet::ComponentSet(int size) :
    m_components(size),
    m_properties(size)
{
}

bool ComponentSet::is_valid() const
{
    if(int(m_components.size()) != m_properties.size())
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }

    for(int i=0; i<this->size(); ++i)
    {
        if(!m_components[i].is_valid())
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    return m_properties._is_valid();
}

// IO --------------------------------------------------------------------------

std::ostream& ComponentSet::write(std::ostream& os) const
{
    PDPC_DEBUG_ASSERT(this->is_valid());

    const int size = this->size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(int));

    for(int i=0; i<size; ++i)
    {
        m_components[i].write(os);
    }
    m_properties.write(os);

    return os;
}

std::istream& ComponentSet::read(std::istream& is)
{
    int size = -1;
    is.read(reinterpret_cast<char*>(&size), sizeof(int));

    PDPC_DEBUG_ASSERT(size >= 0);

    m_components.resize(size);
    for(int i=0; i<size; ++i)
    {
        m_components[i].read(is);
    }
    m_properties.read(is);

    PDPC_DEBUG_ASSERT(this->is_valid());
    return is;
}


// Accessors -------------------------------------------------------------------

int ComponentSet::size() const
{
    return m_components.size();
}

bool ComponentSet::empty() const
{
    return m_components.empty();
}

const Component& ComponentSet::component(int i) const
{
    PDPC_DEBUG_ASSERT(i < this->size());
    return m_components[i];
}

Component& ComponentSet::component(int i)
{
    PDPC_DEBUG_ASSERT(i < this->size());
    return m_components[i];
}

const Component& ComponentSet::operator[](int i) const
{
    return this->component(i);
}

Component& ComponentSet::operator[](int i)
{
    return this->component(i);
}

const Component& ComponentSet::front() const
{
    return this->component(0);
}

Component& ComponentSet::front()
{
    return this->component(0);
}

const std::vector<Component>& ComponentSet::data() const
{
    return m_components;
}

std::vector<Component>& ComponentSet::data()
{
    return m_components;
}

const Component& ComponentSet::back() const
{
    return this->component(this->size()-1);
}

Component& ComponentSet::back()
{
    return this->component(this->size()-1);
}

const property_map& ComponentSet::properties() const
{
    return m_properties;
}

property_map& ComponentSet::properties()
{
    return m_properties;
}

// Modifiers -------------------------------------------------------------------

void ComponentSet::clear()
{
    m_components.clear();
    m_properties.clear();
}

void ComponentSet::resize(int count)
{
    m_components.resize(count);
    m_properties.resize(count);
}

void ComponentSet::push_back()
{
    m_components.emplace_back();
    m_properties.emplace_back();
}

} // namespace pdpc
