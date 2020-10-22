#pragma once

#include <PDPC/ScaleSpace/ScaleSampling.h>

namespace pdpc {

template<class DataT>
class ScaleSpaceStack
{
    // Types -------------------------------------------------------------------
public:
    using Data      = DataT;
    using DataStack = std::vector<Data>;

    // ScaleSpaceStack ---------------------------------------------------------
public:
    inline ScaleSpaceStack();
    inline ScaleSpaceStack(int scale_count);
    inline ScaleSpaceStack(int scale_count, const DataT& init);
    inline ScaleSpaceStack(const ScaleSampling& scale_sampling);

    // IO ----------------------------------------------------------------------
public:
    inline std::ostream& write(std::ostream& os) const;
    inline std::istream& read(std::istream& is);

    // Capacity ----------------------------------------------------------------
public:
    inline int size() const;
    inline bool empty() const;

    // Modifiers ---------------------------------------------------------------
public:
    inline void clear();
    inline void resize(int scale_count);

    inline void set_scale_sampling(const ScaleSampling& scale_sampling);

    // Accessors ---------------------------------------------------------------
public:
    inline const Scalar& scale(int idx_scale) const;
    inline       Scalar& scale(int idx_scale);

    inline const ScaleSampling& scale_sampling() const;
    inline       ScaleSampling& scale_sampling();

    inline const Data& operator[](int idx_scale) const;
    inline       Data& operator[](int idx_scale);

    inline const Data& data(int idx_scale) const;
    inline       Data& data(int idx_scale);

    inline const DataStack& data() const;
    inline       DataStack& data();

    // Process -----------------------------------------------------------------
public:
    template<class FuncT>
    inline void for_each(FuncT&& f);

    template<class FuncT>
    inline void for_each(FuncT&& f) const;

    // Data --------------------------------------------------------------------
protected:
    ScaleSampling m_scale_sampling;
    DataStack     m_data;

}; // class ScaleSpaceStack

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// ScaleSpaceStack ---------------------------------------------------------

template<typename T>
ScaleSpaceStack<T>::ScaleSpaceStack() :
    m_scale_sampling(),
    m_data()
{
}

template<typename T>
ScaleSpaceStack<T>::ScaleSpaceStack(int scale_count) :
    m_scale_sampling(scale_count),
    m_data(scale_count)
{
}

template<typename T>
ScaleSpaceStack<T>::ScaleSpaceStack(int scale_count, const Data& init) :
    m_scale_sampling(scale_count),
    m_data(scale_count,init)
{
}

template<typename T>
ScaleSpaceStack<T>::ScaleSpaceStack(const ScaleSampling& scale_sampling) :
    m_scale_sampling(scale_sampling),
    m_data(scale_sampling.size())
{
}

// IO --------------------------------------------------------------------------

template<typename T>
std::ostream& ScaleSpaceStack<T>::write(std::ostream& os) const
{
    int size = this->size();
    os.write(reinterpret_cast<char*>(&size), sizeof(int));

    for(int i=0; i<size; ++i)
    {
        this->operator[](i).write(os);
    }
    m_scale_sampling.write(os);
    return os;
}

template<typename T>
std::istream& ScaleSpaceStack<T>::read(std::istream& is)
{
    int size = 0;
    is.read(reinterpret_cast<char*>(&size), sizeof(int));

    this->resize(size);
    for(int i=0; i<size; ++i)
    {
        this->operator[](i).read(is);
    }
    m_scale_sampling.read(is);
    return is;
}

// Capacity --------------------------------------------------------------------

template<typename T>
int ScaleSpaceStack<T>::size() const
{
    return m_data.size();
}

template<typename T>
bool ScaleSpaceStack<T>::empty() const
{
    return m_data.empty();
}

// Modifiers -------------------------------------------------------------------

template<typename T>
void ScaleSpaceStack<T>::clear()
{
    m_scale_sampling.clear();
    m_data.clear();
}

template<typename T>
void ScaleSpaceStack<T>::resize(int scale_count)
{
    m_scale_sampling.resize(scale_count);
    m_data.resize(scale_count);
}

template<typename T>
void ScaleSpaceStack<T>::set_scale_sampling(const ScaleSampling& scale_sampling)
{
    m_scale_sampling = scale_sampling;
    m_data.resize(scale_sampling.size());
}

// Accessors -------------------------------------------------------------------

template<typename T>
const Scalar& ScaleSpaceStack<T>::scale(int idx_scale) const
{
    return m_scale_sampling[idx_scale];
}

template<typename T>
Scalar& ScaleSpaceStack<T>::scale(int idx_scale)
{
    return m_scale_sampling[idx_scale];
}

template<typename T>
const ScaleSampling& ScaleSpaceStack<T>::scale_sampling() const
{
    return m_scale_sampling;
}

template<typename T>
ScaleSampling& ScaleSpaceStack<T>::scale_sampling()
{
    return m_scale_sampling;
}

template<typename T>
const T& ScaleSpaceStack<T>::operator[](int idx_scale) const
{
    return data(idx_scale);
}

template<typename T>
T& ScaleSpaceStack<T>::operator[](int idx_scale)
{
    return data(idx_scale);
}

template<typename T>
const T& ScaleSpaceStack<T>::data(int idx_scale) const
{
    return m_data[idx_scale];
}


template<typename T>
T& ScaleSpaceStack<T>::data(int idx_scale)
{
    return m_data[idx_scale];
}

template<typename T>
const typename ScaleSpaceStack<T>::DataStack& ScaleSpaceStack<T>::data() const
{
    return m_data;
}

template<typename T>
typename ScaleSpaceStack<T>::DataStack& ScaleSpaceStack<T>::data()
{
    return m_data;
}

// Process ---------------------------------------------------------------------

template<typename T>
template<class FuncT>
void ScaleSpaceStack<T>::for_each(FuncT&& f)
{
    for(Data& d : m_data)
        f(d);
}

template<typename T>
template<class FuncT>
void ScaleSpaceStack<T>::for_each(FuncT&& f) const
{
    for(const Data& d : m_data)
        f(d);
}

} // namespace pdpc
