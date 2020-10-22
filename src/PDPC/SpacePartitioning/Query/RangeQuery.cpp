#include <PDPC/SpacePartitioning/Query/RangeQuery.h>

#include <cmath>

namespace pdpc {

RangeQuery::RangeQuery() :
    m_squared_radius(0)
{
}

RangeQuery::RangeQuery(Scalar radius) :
    m_squared_radius(radius * radius)
{
}

Scalar RangeQuery::radius() const
{
    return std::sqrt(m_squared_radius);
}

Scalar RangeQuery::squared_radius() const
{
    return m_squared_radius;
}

void RangeQuery::set_radius(Scalar radius)
{
    m_squared_radius = radius * radius;
}

} // namespace pdpc
