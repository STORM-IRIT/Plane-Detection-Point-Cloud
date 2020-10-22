#include <PDPC/PointCloud/Point.h>
#include <PDPC/PointCloud/PointCloud.h>

namespace pdpc {

ConstPoint::ConstPoint() :
    m_point_cloud(nullptr),
    m_index(-1)
{
}

ConstPoint::ConstPoint(const PointCloud* point_cloud, int index) :
    m_point_cloud(point_cloud),
    m_index(index)
{
}

int ConstPoint::index() const
{
    return m_index;
}

int& ConstPoint::index()
{
    return m_index;
}

const Vector3& ConstPoint::pos() const
{
    return m_point_cloud->point(m_index);
}

const Vector3& ConstPoint::point() const
{
    return m_point_cloud->point(m_index);
}

const Vector3& ConstPoint::normal() const
{
    return m_point_cloud->normal(m_index);
}

const Vector4& ConstPoint::color() const
{
    return m_point_cloud->color(m_index);
}

////////////////////////////////////////////////////////////////////////////////

Point::Point() :
    ConstPoint()
{
}

Point::Point(PointCloud* point_cloud, int index) :
    ConstPoint(point_cloud, index)
{
}

Vector3& Point::pos()
{
    return point_cloud()->point(m_index);
}

Vector3& Point::point()
{
    return point_cloud()->point(m_index);
}

Vector3& Point::normal()
{
    return point_cloud()->normal(m_index);
}

Vector4& Point::color()
{
    return point_cloud()->color(m_index);
}

const Vector3& Point::pos() const
{
    return m_point_cloud->point(m_index);
}

const Vector3& Point::point() const
{
    return m_point_cloud->point(m_index);
}

const Vector3& Point::normal() const
{
    return m_point_cloud->normal(m_index);
}

const Vector4& Point::color() const
{
    return m_point_cloud->color(m_index);
}

PointCloud* Point::point_cloud()
{
    return const_cast<PointCloud*>(m_point_cloud);
}

} // namespace pdpc
