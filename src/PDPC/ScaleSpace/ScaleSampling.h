#pragma once

#include <PDPC/Common/Defines.h>

#include <vector>
#include <iostream>

namespace pdpc {

//!
//! \brief The ScaleSampling class contains an ordered set of positive scalar
//! values defining the sampling of the scale-space
//!
class ScaleSampling
{
    // Types -------------------------------------------------------------------
public:
    using ScalarArray   = std::vector<Scalar>;

    // ScaleSampling -----------------------------------------------------------
public:
    ScaleSampling();
    ScaleSampling(int count);

    bool is_valid() const;

    // In/Out ------------------------------------------------------------------
public:
    std::ostream& write(std::ostream& os) const;
    std::istream& read(std::istream& is);

    bool save(const std::string& filename, bool verbose = true) const;
    bool load(const std::string& filename, bool verbose = true);

    // Modifiers ---------------------------------------------------------------
public:
    inline void clear();
    inline void resize(int size);
    inline void multiply(Scalar s);
    inline void multiply_by(Scalar s);

    // Accessors ---------------------------------------------------------------
public:    
    inline int  size() const;
    inline bool empty() const;

    inline const Scalar& operator [] (int i) const;
    inline       Scalar& operator [] (int i);

    inline const ScalarArray& scales() const;
    inline       ScalarArray& scales();

    // Basic info --------------------------------------------------------------
public:
    inline const Scalar& min() const;
    inline const Scalar& max() const;

    inline Scalar range() const;

    // Sampling ----------------------------------------------------------------
public:
    //!
    //! \brief sample logarithmically generates a number of scalar samples in a
    //! given range so that each interval between two consecutive samples is
    //! equal to the previous one multiplied by a given base factor
    //!
    //! If the base is equal to 1, then the sampling is uniform.
    //! It the base is equal to 3, then the samples are agglutinated around min.
    //! It the base is equal to 0.3, then the samples are agglutinated around max.
    //! The first sample is always equal to min.
    //! The last sample is always equal to max unless only 1 sample is required.
    //! If only 1 sample is required, it is equal to the min value.
    //!
    //! \param min the minimal scale value
    //! \param max the maximal scale value
    //! \param count the expected number of scales
    //! \param base the base factor for the logarithmic sampling
    //!
    void sample(Scalar min   = 0.,
                Scalar max   = 1.,
                int    count = 10,
                Scalar base  = 1.);

    //!
    //! \brief log_sample is the actual logarithmic sampling algorithm
    //!
    //! s_{0}   = smin
    //! s_{n+1} = \alpha * s_{n}
    //!
    //! So :     s_{n} = \alpha^n * smin
    //!
    //!
    //! s_{N-1} = smax    =>    \alpha = (smax/smin)^{1/(N-1)}
    //!
    //! So :     s_{n} = (smax/smin)^{n/(N-1)} * smin
    //!
    //! \return \alpha
    //!
    Scalar log_sample(Scalar scale_min, Scalar scale_max, int scale_count);

    // Data --------------------------------------------------------------------
protected:
    //!
    //! \brief m_scales the ordered set of positive scales
    //!
    ScalarArray m_scales;

}; // class ScaleSampling

} // namespace pdpc

#include <PDPC/ScaleSpace/ScaleSampling.inl>
