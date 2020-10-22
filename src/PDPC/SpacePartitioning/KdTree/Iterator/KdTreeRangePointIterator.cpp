#include <PDPC/SpacePartitioning/KdTree/Iterator/KdTreeRangePointIterator.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeRangePointQuery.h>

namespace pdpc {

KdTreeRangePointIterator::KdTreeRangePointIterator() :
    m_query(nullptr),
    m_index(-1),
    m_start(0),
    m_end(0)
{
}

KdTreeRangePointIterator::KdTreeRangePointIterator(KdTreeRangePointQuery* query) :
    m_query(query),
    m_index(-1),
    m_start(0),
    m_end(0)
{
}

KdTreeRangePointIterator::KdTreeRangePointIterator(KdTreeRangePointQuery* query, int index) :
    m_query(query),
    m_index(index),
    m_start(0),
    m_end(0)
{
}

bool KdTreeRangePointIterator::operator !=(const KdTreeRangePointIterator& other) const
{
    return m_index != other.m_index;
}

void KdTreeRangePointIterator::operator ++()
{
    m_query->advance(*this);
}

int KdTreeRangePointIterator::operator * () const
{
    return m_index;
}

} // namespace pdpc
