#pragma once

#include <PDPC/Common/Defines.h>

namespace pdpc {

class PointCloud;

class ConstPoint
{
public:
    enum{ Dim        = 3};
    using Scalar     = pdpc::Scalar;
    using VectorType = Vector3;
    using MatrixType = Matrix3;

public:
    ConstPoint();
    ConstPoint(const PointCloud* point_cloud, int index);

public:
    int  index() const;
    int& index();

public:
    const Vector3& pos() const;
    const Vector3& point() const;
    const Vector3& normal() const;
    const Vector4& color() const;

protected:
    const PointCloud* m_point_cloud;
    int m_index;
};

////////////////////////////////////////////////////////////////////////////////

class Point : public ConstPoint
{
public:
    Point();
    Point(PointCloud* point_cloud, int index);

public:
    Vector3& pos();
    Vector3& point();
    Vector3& normal();
    Vector4& color();

public:
    const Vector3& pos() const;
    const Vector3& point() const;
    const Vector3& normal() const;
    const Vector4& color() const;

protected:
    PointCloud* point_cloud();
};

} // namespace pdpc
