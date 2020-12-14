#include <PDPC/MultiScaleFeatures/MultiScaleFeatures.h>

namespace pdpc {

MultiScaleFeatures::MultiScaleFeatures(int point_count, int scale_count) :
    m_point_count(point_count),
    m_scale_count(scale_count),
    m_normals(point_count * scale_count),
    m_curvatures(point_count * scale_count)
{
}

bool MultiScaleFeatures::save(const std::string& filename) const
{
    PDPC_TODO;
    return false;
}

bool MultiScaleFeatures::load(const std::string& filename)
{
    PDPC_TODO;
    return false;
}

void MultiScaleFeatures::clear()
{
    m_point_count = 0;
    m_scale_count = 0;
    m_normals.clear();
    m_curvatures.clear();
}

void MultiScaleFeatures::resize(int point_count, int scale_count)
{
    m_point_count = point_count;
    m_scale_count = scale_count;
    m_normals.resize(point_count * scale_count);
    m_curvatures.resize(point_count * scale_count);
}

} // namespace pdpc
