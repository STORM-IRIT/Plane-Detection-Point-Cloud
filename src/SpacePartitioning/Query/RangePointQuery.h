#pragma once

#include <SpacePartitioning/Query/RangeQuery.h>
#include <SpacePartitioning/Query/PointQuery.h>

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
