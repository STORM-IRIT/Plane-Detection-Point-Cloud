#pragma once

#include <PDPC/SpacePartitioning/Query/IndexQuery.h>
#include <PDPC/SpacePartitioning/Query/KNearestQuery.h>

namespace pdpc {

class KNearestIndexQuery : public IndexQuery,
                           public KNearestQuery
{
public:
    KNearestIndexQuery();
    KNearestIndexQuery(int k);
    KNearestIndexQuery(int k, int index);
};

} // namespace pdpc
