#include <PDPC/SpacePartitioning/KdTree/Iterator/KdTreeNearestIndexIterator.h>

namespace pdpc {

KdTreeNearestIndexIterator::KdTreeNearestIndexIterator() :
    m_index(-1)
{
}

KdTreeNearestIndexIterator::KdTreeNearestIndexIterator(int index) :
    m_index(index)
{
}

bool KdTreeNearestIndexIterator::operator !=(const KdTreeNearestIndexIterator& other) const
{
    return m_index != other.m_index;
}

void KdTreeNearestIndexIterator::operator ++()
{
    ++m_index;
}

int KdTreeNearestIndexIterator::operator * () const
{
    return m_index;
}

} // namespace pdpc
