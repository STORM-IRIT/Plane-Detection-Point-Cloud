#pragma once

#include <PDPC/Common/Defines.h>

namespace pdpc {

class RangeQuery
{
public:
    RangeQuery();
    RangeQuery(Scalar radius);

public:
    Scalar radius() const;
    Scalar squared_radius() const;
    void set_radius(Scalar radius);

protected:
    Scalar m_squared_radius;
};

} // namespace pdpc
