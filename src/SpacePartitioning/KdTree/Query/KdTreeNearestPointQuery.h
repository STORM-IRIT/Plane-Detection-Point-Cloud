#pragma once

#include <SpacePartitioning/Query/NearestPointQuery.h>
#include <SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <SpacePartitioning/KdTree/Iterator/KdTreeNearestPointIterator.h>

namespace pdpc {

class KdTreeNearestPointQuery : public KdTreeQuery,
                                public NearestPointQuery
{
public:
    KdTreeNearestPointQuery();
    KdTreeNearestPointQuery(const KdTree* kdtree);
    KdTreeNearestPointQuery(const KdTree* kdtree, const Vector3& point);

public:
    KdTreeNearestPointIterator begin();
    KdTreeNearestPointIterator end();

protected:
    void search();
};

} // namespace pdpc
