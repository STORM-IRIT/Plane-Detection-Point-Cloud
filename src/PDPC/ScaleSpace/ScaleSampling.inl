#include <PDPC/ScaleSpace/ScaleSampling.h>

namespace pdpc {

// Modifiers -------------------------------------------------------------------

void ScaleSampling::clear()
{
    m_scales.clear();
}

void ScaleSampling::resize(int size)
{
    m_scales.resize(size);
}

void ScaleSampling::multiply(Scalar s)
{
    for(Scalar& scale : m_scales)
    {
        scale *= s;
    }
}

void ScaleSampling::multiply_by(Scalar s)
{
    for(Scalar& scale : m_scales)
    {
        scale *= s;
    }
}

// Accessors -------------------------------------------------------------------

int ScaleSampling::size() const
{
    return m_scales.size();
}

bool ScaleSampling::empty() const
{
    return m_scales.empty();
}

const Scalar& ScaleSampling::operator [] (int i) const
{
    return m_scales[i];
}

Scalar& ScaleSampling::operator [] (int i)
{
    return m_scales[i];
}

const ScaleSampling::ScalarArray& ScaleSampling::scales() const
{
    return m_scales;
}

ScaleSampling::ScalarArray& ScaleSampling::scales()
{
    return m_scales;
}

// Basic info ------------------------------------------------------------------

const Scalar& ScaleSampling::min() const
{
    return m_scales.front();
}

const Scalar& ScaleSampling::max() const
{
    return m_scales.back();
}

Scalar ScaleSampling::range() const
{
    return max()-min();
}

} // namespace pdpc
