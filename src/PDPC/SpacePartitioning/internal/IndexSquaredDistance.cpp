#include <PDPC/SpacePartitioning/internal/IndexSquaredDistance.h>

namespace pdpc {

bool IndexSquaredDistance::operator < (const IndexSquaredDistance& other) const
{
    return squared_distance < other.squared_distance;
}

} // namespace pdpc
