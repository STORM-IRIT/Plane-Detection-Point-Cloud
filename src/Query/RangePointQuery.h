#pragma once

#include <Query/RangeQuery.h>
#include <Query/PointQuery.h>

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
