#pragma once

#include <Query/IndexQuery.h>
#include <Query/NearestQuery.h>

namespace pdpc {

class NearestIndexQuery : public IndexQuery,
                          public NearestQuery
{
public:
    NearestIndexQuery();
    NearestIndexQuery(int index);
};

} // namespace pdpc
