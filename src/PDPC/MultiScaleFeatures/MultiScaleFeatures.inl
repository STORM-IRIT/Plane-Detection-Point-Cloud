#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>

namespace pdpc {

const Vector3& MultiScaleFeatures::normal(int i, int j) const
{
    return m_normals[index(i,j)];
}

Vector3& MultiScaleFeatures::normal(int i, int j)
{
    return m_normals[index(i,j)];
}

const Vector2& MultiScaleFeatures::curvatures(int i, int j) const
{
    return m_curvatures[index(i,j)];
}

Vector2& MultiScaleFeatures::curvatures(int i, int j)
{
    return m_curvatures[index(i,j)];
}

const Scalar& MultiScaleFeatures::k1(int i, int j) const
{
    return curvatures(i,j)[0];
}

Scalar& MultiScaleFeatures::k1(int i, int j)
{
    return curvatures(i,j)[0];
}

const Scalar& MultiScaleFeatures::k2(int i, int j) const
{
    return curvatures(i,j)[1];
}

Scalar& MultiScaleFeatures::k2(int i, int j)
{
    return curvatures(i,j)[1];
}

int MultiScaleFeatures::index(int i, int j) const
{
    PDPC_DEBUG_ASSERT(0 <= i && i < m_point_count);
    PDPC_DEBUG_ASSERT(0 <= j && j < m_scale_count);
    return j * m_point_count + i;
}

Scalar MultiScaleFeatures::plane_dev(int i, int j) const
{
    return std::sqrt( k1(i,j)*k1(i,j) + k2(i,j)*k2(i,j) );
}

} // namespace pdpc
