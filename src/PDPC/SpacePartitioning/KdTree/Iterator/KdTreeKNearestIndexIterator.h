#pragma once

#include <PDPC/SpacePartitioning/Query/KNearestQuery.h>

namespace pdpc {

class KdTreeKNearestIndexIterator
{
public:
    KdTreeKNearestIndexIterator();
    KdTreeKNearestIndexIterator(limited_priority_queue<IndexSquaredDistance>::iterator iterator);

public:
    bool operator !=(const KdTreeKNearestIndexIterator& other) const;
    void operator ++();
    int  operator * () const;
    void operator +=(int i);

protected:
    limited_priority_queue<IndexSquaredDistance>::iterator m_iterator;
};

} // namespace pdpc
