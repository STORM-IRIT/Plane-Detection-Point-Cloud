#pragma once

#include <vector>
#include <algorithm>

namespace pdpc {

template<typename T, class A>
inline void erase(std::vector<T,A>& vec, const std::vector<bool>& to_erase);

template<typename T, class A>
inline void keep(std::vector<T,A>& vec, const std::vector<bool>& to_keep);

template<typename T, class A>
inline void keep(std::vector<T,A>& vec, const std::vector<int>& to_keep);

template<typename T, class A>
inline void keep(const std::vector<T,A>& vec, std::vector<T,A>& res, const std::vector<int>& to_keep);

template<typename T, class A>
inline void keep(const std::vector<T,A>& vec, std::vector<T,A>& res, const std::vector<bool>& to_keep);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<typename T, class A>
void erase(std::vector<T,A>& vec, const std::vector<bool>& to_erase)
{
    PDPC_DEBUG_ASSERT(vec.size() == to_erase.size());

    const size_t size = vec.size();

    size_t j = 0;
    for(size_t i = 0 ; i < size; ++i)
    {
        if(!to_erase[i])
        {
            if(i != j)
            {
                vec[j] = std::move(vec[i]);
            }
            ++j;
        }
    }

    vec.erase(vec.begin() + j, vec.end());
}

template<typename T, class A>
void keep(std::vector<T,A>& vec, const std::vector<bool>& to_keep)
{
    PDPC_DEBUG_ASSERT(vec.size() == to_keep.size());

    const size_t size = vec.size();

    size_t j = 0;
    for(size_t i = 0 ; i < size; ++i)
    {
        if(to_keep[i])
        {
            vec[j++] = std::move(vec[i]);
        }
    }

    vec.erase(vec.begin() + j, vec.end());
}

template<typename T, class A>
void keep(std::vector<T,A>& vec, const std::vector<int>& to_keep)
{
    PDPC_DEBUG_ASSERT(to_keep.size() <= vec.size());
    PDPC_DEBUG_ASSERT(std::is_sorted(to_keep.begin(), to_keep.end()));

    for(size_t i = 0; i < to_keep.size(); ++i)
    {
        vec[i] = std::move(vec[to_keep[i]]);
    }

    vec.erase(vec.begin() + to_keep.size(), vec.end());
}

template<typename T, class A>
void keep(const std::vector<T,A>& vec, std::vector<T,A>& res, const std::vector<int>& to_keep)
{
    PDPC_DEBUG_ASSERT(to_keep.size() <= vec.size());
    PDPC_DEBUG_ASSERT(std::is_sorted(to_keep.begin(), to_keep.end()));

    res.resize(to_keep.size());
    for(size_t i = 0; i < to_keep.size(); ++i)
    {
        res[i] = std::move(vec[to_keep[i]]);
    }
}

template<typename T, class A>
void keep(const std::vector<T,A>& vec, std::vector<T,A>& res, const std::vector<bool>& to_keep)
{
    PDPC_DEBUG_ASSERT(vec.size() == to_keep.size());

    const size_t size = vec.size();
    const size_t new_size = std::count(to_keep.begin(), to_keep.end(), true);

    res.resize(new_size);

    size_t j = 0;
    for(size_t i = 0 ; i < size; ++i)
    {
        if(to_keep[i])
        {
            res[j++] = std::move(vec[i]);
        }
    }
}

} // namespace pdpc

