#pragma once

#include <PDPC/SpacePartitioning/Query/PointQuery.h>
#include <PDPC/SpacePartitioning/Query/NearestQuery.h>

namespace pdpc {

class NearestPointQuery : public PointQuery,
                          public NearestQuery
{
public:
    NearestPointQuery();
    NearestPointQuery(const Vector3& point);
};

} // namespace pdpc
