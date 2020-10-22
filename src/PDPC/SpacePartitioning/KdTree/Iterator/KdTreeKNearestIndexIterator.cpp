#include <PDPC/SpacePartitioning/KdTree/Iterator/KdTreeKNearestIndexIterator.h>

namespace pdpc {

KdTreeKNearestIndexIterator::KdTreeKNearestIndexIterator() :
    m_iterator()
{
}

KdTreeKNearestIndexIterator::KdTreeKNearestIndexIterator(limited_priority_queue<IndexSquaredDistance>::iterator iterator) :
    m_iterator(iterator)
{
}

bool KdTreeKNearestIndexIterator::operator !=(const KdTreeKNearestIndexIterator& other) const
{
    return m_iterator != other.m_iterator;
}

void KdTreeKNearestIndexIterator::operator ++()
{
    ++m_iterator;
}

int KdTreeKNearestIndexIterator::operator * () const
{
    return m_iterator->index;
}

void KdTreeKNearestIndexIterator::operator +=(int i)
{
    m_iterator += i;
}

} // namespace pdpc
