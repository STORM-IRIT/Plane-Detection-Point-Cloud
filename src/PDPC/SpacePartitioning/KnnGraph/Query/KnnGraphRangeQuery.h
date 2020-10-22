#pragma once

#include <PDPC/SpacePartitioning/Query/RangeIndexQuery.h>
#include <PDPC/SpacePartitioning/KnnGraph/Iterator/KnnGraphRangeIterator.h>

#include <vector>
#include <stack>

namespace pdpc {

class KnnGraph;

class KnnGraphRangeQuery : public RangeIndexQuery
{
protected:
    friend class KnnGraphRangeIterator;

public:
    KnnGraphRangeQuery();
    KnnGraphRangeQuery(const KnnGraph* graph);
    KnnGraphRangeQuery(const KnnGraph* graph, Scalar radius);
    KnnGraphRangeQuery(const KnnGraph* graph, Scalar radius, int index);

public:
    KnnGraphRangeIterator begin();
    KnnGraphRangeIterator end();

protected:
    void initialize(KnnGraphRangeIterator& iterator);
    void advance(KnnGraphRangeIterator& iterator);

protected:
    const KnnGraph*   m_graph;
    std::vector<bool> m_flag;
    std::stack<int>   m_stack;
};

} // namespace pdpc
