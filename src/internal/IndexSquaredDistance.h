#pragma once

namespace pdpc {

struct IndexSquaredDistance
{
    int index;
    float squared_distance;

    bool operator < (const IndexSquaredDistance& other) const;
};

} // namespace pdpc
