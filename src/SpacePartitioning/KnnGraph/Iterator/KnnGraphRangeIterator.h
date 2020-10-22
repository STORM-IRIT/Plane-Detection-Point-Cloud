#pragma once

namespace pdpc {

class KnnGraphRangeQuery;

class KnnGraphRangeIterator
{
protected:
    friend class KnnGraphRangeQuery;

public:
    KnnGraphRangeIterator();
    KnnGraphRangeIterator(KnnGraphRangeQuery* query);
    KnnGraphRangeIterator(KnnGraphRangeQuery* query, int index);

public:
    bool operator != (const KnnGraphRangeIterator& other) const;
    void operator ++ ();
    int  operator *  () const;

protected:
    KnnGraphRangeQuery* m_query;
    int m_index;
};

} // namespace pdpc
