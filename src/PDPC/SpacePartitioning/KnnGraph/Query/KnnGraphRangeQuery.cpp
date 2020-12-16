#include <PDPC/SpacePartitioning/KnnGraph/Query/KnnGraphRangeQuery.h>
#include <PDPC/SpacePartitioning/KnnGraph.h>
#include <PDPC/Common/Assert.h>

namespace pdpc {

KnnGraphRangeQuery::KnnGraphRangeQuery() :
    RangeIndexQuery(),
    m_graph(nullptr),
    m_flag(),
    m_stack()
{
}

KnnGraphRangeQuery::KnnGraphRangeQuery(const KnnGraph* graph) :
    RangeIndexQuery(),
    m_graph(graph),
    m_flag(graph->size()),
    m_stack()
{
}

KnnGraphRangeQuery::KnnGraphRangeQuery(const KnnGraph* graph, Scalar radius) :
    RangeIndexQuery(radius),
    m_graph(graph),
    m_flag(graph->size()),
    m_stack()
{
}

KnnGraphRangeQuery::KnnGraphRangeQuery(const KnnGraph* graph, Scalar radius, int index) :
    RangeIndexQuery(radius, index),
    m_graph(graph),
    m_flag(graph->size()),
    m_stack()
{
}

KnnGraphRangeIterator KnnGraphRangeQuery::begin()
{
    KnnGraphRangeIterator it(this);
    this->initialize(it);
    this->advance(it);
    return it;
}

KnnGraphRangeIterator KnnGraphRangeQuery::end()
{
    return KnnGraphRangeIterator(this, m_graph->size());
}

void KnnGraphRangeQuery::initialize(KnnGraphRangeIterator& iterator)
{
    m_flag.resize(m_graph->size());
    std::fill(m_flag.begin(), m_flag.end(), false);

    PDPC_DEBUG_ASSERT(m_stack.empty());
    m_stack.push(m_index);
    m_flag[m_index] = true;

    iterator.m_index = -1;
}

void KnnGraphRangeQuery::advance(KnnGraphRangeIterator& iterator)
{
    const auto& points  = m_graph->point_data();
    const auto& point   = points[m_index];

    if(m_stack.empty())
    {
        iterator.m_index = m_graph->size();
    }
    else
    {
        int idx_current = m_stack.top();
        m_stack.pop();

        PDPC_DEBUG_ASSERT((point - points[idx_current]).squaredNorm() < m_squared_radius);

        iterator.m_index = idx_current;

        for(int idx_nei : m_graph->k_nearest_neighbors(idx_current))
        {
            if(!m_flag[idx_nei] && (point - points[idx_nei]).squaredNorm() < m_squared_radius)
            {
                m_flag[idx_nei] = true;
                m_stack.push(idx_nei);
            }
        }
    }
}

} // namespace pdpc
