#include <PDPC/SpacePartitioning/KdTree/Iterator/KdTreeKNearestPointIterator.h>

namespace pdpc {

KdTreeKNearestPointIterator::KdTreeKNearestPointIterator() :
    m_iterator()
{
}

KdTreeKNearestPointIterator::KdTreeKNearestPointIterator(limited_priority_queue<IndexSquaredDistance>::iterator iterator) :
    m_iterator(iterator)
{
}

bool KdTreeKNearestPointIterator::operator !=(const KdTreeKNearestPointIterator& other) const
{
    return m_iterator != other.m_iterator;
}

void KdTreeKNearestPointIterator::operator ++()
{
    ++m_iterator;
}

int KdTreeKNearestPointIterator::operator * () const
{
    return m_iterator->index;
}

} // namespace pdpc
