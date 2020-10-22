#pragma once

#include <SpacePartitioning/Query/IndexQuery.h>
#include <SpacePartitioning/Query/KNearestQuery.h>

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
