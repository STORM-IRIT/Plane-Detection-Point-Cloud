#pragma once

#include <Query/KNearestIndexQuery.h>
#include <KdTree/Query/KdTreeQuery.h>
#include <KdTree/Iterator/KdTreeKNearestIndexIterator.h>

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
