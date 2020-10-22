#include <PDPC/SpacePartitioning/Query/RangeIndexQuery.h>

namespace pdpc {

RangeIndexQuery::RangeIndexQuery() :
    IndexQuery(),
    RangeQuery()
{
}

RangeIndexQuery::RangeIndexQuery(Scalar radius) :
    IndexQuery(),
    RangeQuery(radius)
{
}

RangeIndexQuery::RangeIndexQuery(Scalar radius, int index) :
    IndexQuery(index),
    RangeQuery(radius)
{
}

} // namespace pdpc
