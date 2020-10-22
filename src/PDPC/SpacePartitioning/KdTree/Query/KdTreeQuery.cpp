#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeQuery.h>

namespace pdpc {

KdTreeQuery::KdTreeQuery() :
    m_kdtree(nullptr)
{
}

KdTreeQuery::KdTreeQuery(const KdTree* kdtree) :
    m_kdtree(kdtree)
{
}

} // namespace pdpc
