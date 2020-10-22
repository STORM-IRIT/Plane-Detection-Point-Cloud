#include <PDPC/SpacePartitioning/KnnGraph/Query/KnnGraphQuery.h>
#include <PDPC/SpacePartitioning/KnnGraph.h>

namespace pdpc {

KnnGraphQuery::KnnGraphQuery() :
    m_graph(nullptr),
    m_index(-1)
{
}

KnnGraphQuery::KnnGraphQuery(const KnnGraph* graph, int index) :
    m_graph(graph),
    m_index(index)
{
}

KnnGraphQuery::iterator KnnGraphQuery::begin() const
{
    return m_graph->index_data().begin() + m_index * m_graph->k();
}

KnnGraphQuery::iterator KnnGraphQuery::end() const
{
    return m_graph->index_data().begin() + (m_index+1) * m_graph->k();
}

} // namespace pdpc
