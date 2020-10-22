#pragma once

#include <PDPC/SpacePartitioning/Query/IndexQuery.h>
#include <PDPC/SpacePartitioning/Query/NearestQuery.h>

namespace pdpc {

class NearestIndexQuery : public IndexQuery,
                          public NearestQuery
{
public:
    NearestIndexQuery();
    NearestIndexQuery(int index);
};

} // namespace pdpc
