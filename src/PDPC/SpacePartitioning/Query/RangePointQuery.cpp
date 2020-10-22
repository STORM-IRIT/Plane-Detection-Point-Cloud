#include <PDPC/SpacePartitioning/Query/RangePointQuery.h>

namespace pdpc {

RangePointQuery::RangePointQuery() :
    PointQuery(),
    RangeQuery()
{
}

RangePointQuery::RangePointQuery(Scalar radius) :
    PointQuery(),
    RangeQuery(radius)
{
}

RangePointQuery::RangePointQuery(Scalar radius, const Vector3& point) :
    PointQuery(point),
    RangeQuery(radius)
{
}

} // namespace pdpc
