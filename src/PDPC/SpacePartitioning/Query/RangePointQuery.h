#pragma once

#include <PDPC/SpacePartitioning/Query/RangeQuery.h>
#include <PDPC/SpacePartitioning/Query/PointQuery.h>

namespace pdpc {

class RangePointQuery : public PointQuery,
                        public RangeQuery
{
public:
    RangePointQuery();
    RangePointQuery(Scalar radius);
    RangePointQuery(Scalar radius, const Vector3& point);
};

} // namespace pdpc
