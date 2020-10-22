#pragma once

#include <SpacePartitioning/Query/NearestIndexQuery.h>
#include <SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <SpacePartitioning/KdTree/Iterator/KdTreeNearestIndexIterator.h>

namespace pdpc {

class KdTreeNearestIndexQuery : public KdTreeQuery,
                                public NearestIndexQuery
{
public:
    KdTreeNearestIndexQuery();
    KdTreeNearestIndexQuery(const KdTree* kdtree);
    KdTreeNearestIndexQuery(const KdTree* kdtree, int index);

public:
    KdTreeNearestIndexIterator begin();
    KdTreeNearestIndexIterator end();

public:
    const NearestIndexQuery& search();
};

} // namespace pdpc
