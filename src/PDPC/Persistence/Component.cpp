#include <PDPC/Persistence/Component.h>
#include <PDPC/Common/Assert.h>

namespace pdpc {

// Component -------------------------------------------------------------------

Component::Component() :
    m_birth_level(0),
    m_indices(0)
{
}

Component::Component(int index) :
    m_birth_level(0),
    m_indices(index)
{
}

Component::Component(int birth_level, int index) :
    m_birth_level(birth_level),
    m_indices(1,index)
{
}

bool Component::is_valid() const
{
    if(m_birth_level < 0)
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    if(m_indices.empty() && m_birth_level > 0)
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    return true;
}

// IO --------------------------------------------------------------------------

std::ostream& Component::write(std::ostream& os) const
{
    PDPC_DEBUG_ASSERT(this->is_valid());

    const int size = this->size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(int));
    os.write(reinterpret_cast<const char*>(&m_birth_level), sizeof(int));
    os.write(reinterpret_cast<const char*>(m_indices.data()), size*sizeof(int));
    return os;
}

std::istream& Component::read(std::istream& is)
{
    int size = -1;
    is.read(reinterpret_cast<char*>(&size), sizeof(int));
    is.read(reinterpret_cast<char*>(&m_birth_level), sizeof(int));

    PDPC_DEBUG_ASSERT(size >= 0);
    PDPC_DEBUG_ASSERT(m_birth_level >= 0);
    m_indices.resize(size);

    is.read(reinterpret_cast<char*>(m_indices.data()), size*sizeof(int));
    return is;
}

// Accessors -------------------------------------------------------------------

int Component::size() const
{
    return m_indices.size();
}

bool Component::empty() const
{
    return m_indices.empty();
}

int Component::birth_level() const
{
    return m_birth_level;
}

int Component::death_level() const
{
    return this->birth_level() + this->size() - 1;
}

int Component::persistence() const
{
    return this->size();
}

int Component::index(int level) const
{
    return this->index_at_level(level);
}

int Component::operator[] (int level) const
{
    return this->index(level);
}

int Component::level_at(int i) const
{
    PDPC_DEBUG_ASSERT(i < this->size());
    return m_birth_level + i;
}

int Component::index_at(int i) const
{
    PDPC_DEBUG_ASSERT(i < this->size());
    return m_indices[i];
}

// Modifiers -------------------------------------------------------------------

void Component::clear()
{
    m_birth_level = 0;
    m_indices.clear();
}

void Component::set_index(int level, int index)
{
    this->index_at_level(level) = index;
}

void Component::push_back(int index)
{
    m_indices.emplace_back(index);
}

void Component::initialize(int birth_level, int index)
{
    m_birth_level = birth_level;
    m_indices.resize(1);
    m_indices[0] = index;
}

void Component::reserve(int cap)
{
    m_indices.reserve(cap);
}

// Internal --------------------------------------------------------------------

int Component::index_at_level(int level) const
{
    PDPC_DEBUG_ASSERT(this->birth_level() <= level && level <= this->death_level());
    return m_indices[level - m_birth_level];
}

int& Component::index_at_level(int level)
{
    PDPC_DEBUG_ASSERT(this->birth_level() <= level && level <= this->death_level());
    return m_indices[level - m_birth_level];
}

} // namespace pdpc
