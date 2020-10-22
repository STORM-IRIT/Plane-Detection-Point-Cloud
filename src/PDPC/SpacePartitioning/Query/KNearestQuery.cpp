#include <PDPC/SpacePartitioning/Query/KNearestQuery.h>

namespace pdpc {

KNearestQuery::KNearestQuery() :
    m_queue()
{
}

KNearestQuery::KNearestQuery(int k) :
    m_queue(k)
{
}

int KNearestQuery::k() const
{
    return m_queue.capacity();
}

void KNearestQuery::set_k(int k)
{
    m_queue.reserve(k);
}

limited_priority_queue<IndexSquaredDistance>& KNearestQuery::queue()
{
    return m_queue;
}


} // namespace pdpc
