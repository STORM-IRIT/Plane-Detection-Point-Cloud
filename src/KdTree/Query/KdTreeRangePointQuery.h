#pragma once

#include <Query/RangePointQuery.h>
#include <KdTree/Query/KdTreeQuery.h>
#include <KdTree/Iterator/KdTreeRangePointIterator.h>

namespace pdpc {

class KdTreeRangePointQuery : public KdTreeQuery,
                              public RangePointQuery
{
protected:
    friend class KdTreeRangePointIterator;

public:
    KdTreeRangePointQuery();
    KdTreeRangePointQuery(const KdTree* kdtree);
    KdTreeRangePointQuery(const KdTree* kdtree, Scalar radius);
    KdTreeRangePointQuery(const KdTree* kdtree, Scalar radius, const Vector3& point);

public:
    KdTreeRangePointIterator begin();
    KdTreeRangePointIterator end();

protected:
    void initialize(KdTreeRangePointIterator& iterator);
    void advance(KdTreeRangePointIterator& iterator);
};

} // namespace pdpc
