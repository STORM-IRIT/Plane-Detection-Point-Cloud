#pragma once

#include <SpacePartitioning/Query/KNearestPointQuery.h>
#include <SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <SpacePartitioning/KdTree/Iterator/KdTreeKNearestPointIterator.h>

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
