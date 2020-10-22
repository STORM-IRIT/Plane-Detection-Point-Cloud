#pragma once

#include <PDPC/SpacePartitioning/Query/PointQuery.h>
#include <PDPC/SpacePartitioning/Query/KNearestQuery.h>

namespace pdpc {

class KNearestPointQuery : public PointQuery,
                           public KNearestQuery
{
public:
    KNearestPointQuery();
    KNearestPointQuery(const Vector3& point);
    KNearestPointQuery(int k);
    KNearestPointQuery(int k, const Vector3& point);
};

} // namespace pdpc
