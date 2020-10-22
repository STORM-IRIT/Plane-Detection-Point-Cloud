#include <PDPC/SpacePartitioning/Query/NearestPointQuery.h>

namespace pdpc {

NearestPointQuery::NearestPointQuery() :
    PointQuery(),
    NearestQuery()
{
}

NearestPointQuery::NearestPointQuery(const Vector3& point) :
    PointQuery(point),
    NearestQuery()
{
}

} // namespace pdpc
