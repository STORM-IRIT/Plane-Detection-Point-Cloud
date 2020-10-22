#pragma once

#include <limited_priority_queue.h>

#include <internal/IndexSquaredDistance.h>

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
