#pragma once

#include <PDPC/SpacePartitioning/Query/KNearestPointQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Iterator/KdTreeKNearestPointIterator.h>

namespace pdpc {

class KdTreeKNearestPointQuery : public KdTreeQuery,
                                 public KNearestPointQuery
{
public:
    KdTreeKNearestPointQuery();
    KdTreeKNearestPointQuery(const KdTree* kdtree, int k);
    KdTreeKNearestPointQuery(const KdTree* kdtree, int k, const Vector3& point);

public:
    KdTreeKNearestPointIterator begin();
    KdTreeKNearestPointIterator end();

public:
    const limited_priority_queue<IndexSquaredDistance>& search();
};

} // namespace pdpc
