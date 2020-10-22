#pragma once

#include <Query/PointQuery.h>
#include <Query/NearestQuery.h>

namespace pdpc {

class NearestPointQuery : public PointQuery,
                          public NearestQuery
{
public:
    NearestPointQuery();
    NearestPointQuery(const Vector3& point);
};

} // namespace pdpc
