#include <PDPC/SpacePartitioning/Query/NearestIndexQuery.h>

namespace pdpc {

NearestIndexQuery::NearestIndexQuery() :
    IndexQuery(),
    NearestQuery()
{
}

NearestIndexQuery::NearestIndexQuery(int index) :
    IndexQuery(index),
    NearestQuery()
{
}

} // namespace pdpc
