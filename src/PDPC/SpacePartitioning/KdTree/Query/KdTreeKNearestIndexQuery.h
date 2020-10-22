#pragma once

#include <PDPC/SpacePartitioning/Query/KNearestIndexQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Iterator/KdTreeKNearestIndexIterator.h>

namespace pdpc {

class KdTreeKNearestIndexQuery : public KdTreeQuery,
                                 public KNearestIndexQuery
{
public:
    KdTreeKNearestIndexQuery();
    KdTreeKNearestIndexQuery(const KdTree* kdtree, int k);
    KdTreeKNearestIndexQuery(const KdTree* kdtree, int k, int index);

public:
    KdTreeKNearestIndexIterator begin();
    KdTreeKNearestIndexIterator end();

public:
     const limited_priority_queue<IndexSquaredDistance>& search();
};

} // namespace pdpc
