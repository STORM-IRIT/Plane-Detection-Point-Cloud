#include <PDPC/SpacePartitioning/KnnGraph/Iterator/KnnGraphRangeIterator.h>
#include <PDPC/SpacePartitioning/KnnGraph/Query/KnnGraphRangeQuery.h>

namespace pdpc {

KnnGraphRangeIterator::KnnGraphRangeIterator() :
    m_query(nullptr),
    m_index(-1)
{
}

KnnGraphRangeIterator::KnnGraphRangeIterator(KnnGraphRangeQuery* query) :
    m_query(query),
    m_index(-1)
{
}

KnnGraphRangeIterator::KnnGraphRangeIterator(KnnGraphRangeQuery* query, int index) :
    m_query(query),
    m_index(index)
{
}

bool KnnGraphRangeIterator::operator != (const KnnGraphRangeIterator& other) const
{
    return m_index != other.m_index;
}

void KnnGraphRangeIterator::operator ++ ()
{
    m_query->advance(*this);
}

int KnnGraphRangeIterator::operator *  () const
{
    return m_index;
}


} // namespace pdpc
