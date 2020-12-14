#pragma once

#include <algorithm>

namespace pdpc {

//!
//! \brief has_duplicate checks if the given container contains duplicated items
//!
//! Complexity = O(n^2)
//!
template<class ContainerT>
bool has_duplicate(ContainerT container);

template<class RandomIt>
bool has_duplicate(RandomIt first, RandomIt last);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template<class ContainerT>
bool has_duplicate(ContainerT container)
{
    return has_duplicate(std::begin(container), std::end(container));
}

template<class RandomIt>
bool has_duplicate(RandomIt first, RandomIt last)
{
    return std::any_of(first, last, [&](const auto& cur)->bool
    {
        // next is the iterator pointing after the current value cur
        RandomIt next = first + (&cur - &(*first)) + 1;
        return std::find(next, last, cur) != last;
    });
}

} // namespace pdpc
