#pragma once

#include <SpacePartitioning/Query/IndexQuery.h>
#include <SpacePartitioning/Query/NearestQuery.h>

namespace pdpc {

class NearestIndexQuery : public IndexQuery,
                          public NearestQuery
{
public:
    NearestIndexQuery();
    NearestIndexQuery(int index);
};

} // namespace pdpc
