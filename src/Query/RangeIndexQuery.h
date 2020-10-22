#pragma once

#include <Query/RangeQuery.h>
#include <Query/IndexQuery.h>

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
