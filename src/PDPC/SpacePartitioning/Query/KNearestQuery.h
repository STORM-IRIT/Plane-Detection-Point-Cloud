#pragma once

#include <PDPC/SpacePartitioning/internal/IndexSquaredDistance.h>

#include <PDPC/Common/Containers/limited_priority_queue.h>

namespace pdpc {

class KNearestQuery
{
public:
    KNearestQuery();
    KNearestQuery(int k);

public:
    int k() const;
    void set_k(int k);

public:
    limited_priority_queue<IndexSquaredDistance>& queue();

protected:
    limited_priority_queue<IndexSquaredDistance> m_queue;
};

} // namespace pdpc
