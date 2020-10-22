#pragma once

namespace pdpc {

class KdTreeRangePointQuery;

class KdTreeRangePointIterator
{
protected:
    friend class KdTreeRangePointQuery;

public:
    KdTreeRangePointIterator();
    KdTreeRangePointIterator(KdTreeRangePointQuery* query);
    KdTreeRangePointIterator(KdTreeRangePointQuery* query, int index);

public:
    bool operator !=(const KdTreeRangePointIterator& other) const;
    void operator ++();
    int  operator * () const;

protected:
    KdTreeRangePointQuery* m_query;
    int m_index;
    int m_start;
    int m_end;
};

} // namespace pdpc
