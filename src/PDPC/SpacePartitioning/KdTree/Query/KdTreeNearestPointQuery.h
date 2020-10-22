#pragma once

#include <PDPC/SpacePartitioning/Query/NearestPointQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Iterator/KdTreeNearestPointIterator.h>

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
