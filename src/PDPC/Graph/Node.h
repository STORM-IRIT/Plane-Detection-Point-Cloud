#pragma once

#include <vector>

namespace pdpc {

struct Node
{
    std::vector<int> in;
    std::vector<int> out;

    inline bool operator == (const Node& node) const
    {
        return in == node.in && out == node.out;
    }
};

} // namespace pdpc
