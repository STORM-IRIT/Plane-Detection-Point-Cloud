#pragma once

#include <SpacePartitioning/Query/RangeQuery.h>
#include <SpacePartitioning/Query/IndexQuery.h>

namespace pdpc {

class RangeIndexQuery : public IndexQuery,
                        public RangeQuery
{
public:
    RangeIndexQuery();
    RangeIndexQuery(Scalar radius);
    RangeIndexQuery(Scalar radius, int index);
};

} // namespace pdpc
