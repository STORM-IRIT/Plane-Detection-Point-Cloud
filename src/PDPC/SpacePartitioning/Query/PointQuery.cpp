#include <PDPC/SpacePartitioning/Query/PointQuery.h>

namespace pdpc {

PointQuery::PointQuery() :
    m_point(Vector3::Zero())
{
}

PointQuery::PointQuery(const Vector3& point) :
    m_point(point)
{
}

const Vector3& PointQuery::point() const
{
    return m_point;
}

void PointQuery::set_point(const Vector3& point)
{
    m_point = point;
}

} // namespace pdpc
