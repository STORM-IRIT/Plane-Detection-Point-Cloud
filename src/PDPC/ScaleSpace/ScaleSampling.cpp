#include <PDPC/ScaleSpace/ScaleSampling.h>

#include <cmath>

namespace pdpc {

// ScaleSampling ---------------------------------------------------------------

ScaleSampling::ScaleSampling() :
    m_scales()
{
}

ScaleSampling::ScaleSampling(int count) :
    m_scales(count)
{
}

bool ScaleSampling::is_valid() const
{
    for(int i=0; i<size()-1; ++i)
    {
        if(m_scales[i+1] < m_scales[i])
        {
            assert(false);
            return false;
        }
    }
    return true;
}


// In/Out ----------------------------------------------------------------------

std::ostream& ScaleSampling::write(std::ostream& os) const
{
    assert(this->is_valid());
    int size = m_scales.size();
    os.write(reinterpret_cast<char*>(&size), sizeof(int));
    os.write(reinterpret_cast<const char*>(m_scales.data()), size*sizeof(Scalar));
    return os;
}

std::istream& ScaleSampling::read(std::istream& is)
{
    int size = 0;
    is.read(reinterpret_cast<char*>(&size), sizeof(int));
    m_scales.resize(size);
    is.read(reinterpret_cast<char*>(m_scales.data()), size*sizeof(Scalar));
    assert(this->is_valid());
    return is;
}

// Sampling --------------------------------------------------------------------

void ScaleSampling::sample(Scalar min, Scalar max, int count, Scalar base)
{
    m_scales.clear();

    m_scales.resize(count);
    m_scales[0] = min;

    if(std::abs(base-1)<1e-3)
    {
        // uniform sampling
        Scalar delta0 = (max-min)/(count-1);
        for(int i=1; i<count; ++i)
        {
            m_scales[i] = min + delta0*i;
        }
    }
    else
    {
        // log sampling
        Scalar delta0 = (max-min) * (1.-base) / (1.-std::pow(base, count-1));
        for(int i=1; i<count; ++i)
        {
            m_scales[i] = min + delta0*(1.-std::pow(base, i)) / (1.-base);
        }
    }
    assert(this->is_valid());
}

Scalar ScaleSampling::log_sample(Scalar scale_min, Scalar scale_max, int scale_count)
{
    assert(Scalar(0) < scale_min);
    assert(scale_min < scale_max);
    assert(0 < scale_count);

    m_scales.resize(scale_count);

    if(scale_count == 1)
    {
        m_scales[0] = scale_min;
        return Scalar(1);
    }
    else
    {
        Scalar alpha = std::pow(scale_max/scale_min, Scalar(1)/(scale_count-1));

        for(int n=0; n<scale_count; ++n)
        {
            m_scales[n] = std::pow(alpha, n) * scale_min;
        }

        assert(this->is_valid());
        return alpha;
    }
}

} // namespace pdpc
