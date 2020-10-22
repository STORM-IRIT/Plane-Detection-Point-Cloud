#pragma once

#include <SpacePartitioning/Query/RangeIndexQuery.h>
#include <SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <SpacePartitioning/KdTree/Iterator/KdTreeRangeIndexIterator.h>

namespace pdpc {

class KdTreeRangeIndexQuery : public KdTreeQuery,
                              public RangeIndexQuery
{
protected:
    friend class KdTreeRangeIndexIterator;

public:
    KdTreeRangeIndexQuery();
    KdTreeRangeIndexQuery(const KdTree* kdtree);
    KdTreeRangeIndexQuery(const KdTree* kdtree, Scalar radius);
    KdTreeRangeIndexQuery(const KdTree* kdtree, Scalar radius, int index);

public:
    KdTreeRangeIndexIterator begin();
    KdTreeRangeIndexIterator end();

protected:
    void initialize(KdTreeRangeIndexIterator& iterator);
    void advance(KdTreeRangeIndexIterator& iterator);
};

} // namespace pdpc
