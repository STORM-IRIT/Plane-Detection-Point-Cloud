#pragma once

#include <vector>

namespace pdpc {

class KnnGraph;

class KnnGraphQuery
{
public:
    using iterator = std::vector<int>::const_iterator;

public:
    KnnGraphQuery();
    KnnGraphQuery(const KnnGraph* graph, int index);

    iterator begin() const;
    iterator end() const;

protected:
    const KnnGraph* m_graph;
    int m_index;
};

} // namespace pdpc
