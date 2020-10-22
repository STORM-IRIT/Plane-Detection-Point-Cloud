#pragma once

#include <Query/IndexQuery.h>
#include <Query/KNearestQuery.h>

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
