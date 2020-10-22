#include <PDPC/Common/Containers/internal/abstract_vector_index_iterator.h>

namespace pdpc {

abstract_vector_index_iterator::abstract_vector_index_iterator() :
    m_index(0)
{
}

abstract_vector_index_iterator::abstract_vector_index_iterator(const abstract_vector_index_iterator& other) :
    m_index(other.m_index)
{
}

abstract_vector_index_iterator::abstract_vector_index_iterator(int current) :
    m_index(current)
{
}

abstract_vector_index_iterator& abstract_vector_index_iterator::operator += (int n)
{
    m_index += n;
    return *this;
}

abstract_vector_index_iterator& abstract_vector_index_iterator::operator -= (int n)
{
    m_index -= n;
    return *this;
}

abstract_vector_index_iterator& abstract_vector_index_iterator::operator ++ ()
{
    ++m_index;
    return *this;
}

abstract_vector_index_iterator& abstract_vector_index_iterator::operator -- ()
{
    --m_index;
    return *this;
}

abstract_vector_index_iterator abstract_vector_index_iterator::operator + (int n) const
{
    abstract_vector_index_iterator copy(*this);
    copy += n;
    return copy;
}

abstract_vector_index_iterator abstract_vector_index_iterator::operator - (int n) const
{
    abstract_vector_index_iterator copy(*this);
    copy -= n;
    return copy;
}

bool abstract_vector_index_iterator::operator == (const abstract_vector_index_iterator& other) const
{
    return m_index == other.m_index;
}

bool abstract_vector_index_iterator::operator != (const abstract_vector_index_iterator& other) const
{
    return m_index != other.m_index;
}

int abstract_vector_index_iterator::operator * () const
{
    return m_index;
}

int abstract_vector_index_iterator::index() const
{
    return m_index;
}

} // namespace pdpc
