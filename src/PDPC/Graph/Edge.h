#pragma once

namespace pdpc {

struct Edge
{
    int source;
    int target;

    inline bool operator == (const Edge& edge) const
    {
        return source == edge.source && target == edge.target;
    }
};

} // namespace pdpc
