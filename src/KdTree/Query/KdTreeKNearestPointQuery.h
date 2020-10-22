#pragma once

#include <Query/KNearestPointQuery.h>
#include <KdTree/Query/KdTreeQuery.h>
#include <KdTree/Iterator/KdTreeKNearestPointIterator.h>

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
