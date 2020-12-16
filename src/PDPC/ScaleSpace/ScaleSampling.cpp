#include <PDPC/ScaleSpace/ScaleSampling.h>
#include <PDPC/Common/Log.h>
#include <PDPC/Common/Assert.h>

#include <cmath>
#include <fstream>

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
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    return true;
}


// In/Out ----------------------------------------------------------------------

std::ostream& ScaleSampling::write(std::ostream& os) const
{
    PDPC_DEBUG_ASSERT(this->is_valid());
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
    PDPC_DEBUG_ASSERT(this->is_valid());
    return is;
}

bool ScaleSampling::save(const std::string& filename, bool v) const
{
    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        error().iff(v) << "Failed to open output scales file " << filename;
        return false;
    }

    for(const Scalar s : m_scales)
    {
        ofs << s << " ";
    }

    info().iff(v) << m_scales.size() << " scales saved to " << filename;

    return true;
}

bool ScaleSampling::load(const std::string& filename, bool v)
{
    this->clear();

    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        error().iff(v) << "Failed to open input scales file " << filename;
        return false;
    }

    Scalar s;
    while(ifs >> s)
        m_scales.push_back(s);

    info().iff(v) << m_scales.size() << " scales loaded from " << filename;

    return true;
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
    PDPC_DEBUG_ASSERT(this->is_valid());
}

Scalar ScaleSampling::log_sample(Scalar scale_min, Scalar scale_max, int scale_count)
{
    PDPC_DEBUG_ASSERT(Scalar(0) < scale_min);
    PDPC_DEBUG_ASSERT(scale_min < scale_max);
    PDPC_DEBUG_ASSERT(0 < scale_count);

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

        PDPC_DEBUG_ASSERT(this->is_valid());
        return alpha;
    }
}

} // namespace pdpc
