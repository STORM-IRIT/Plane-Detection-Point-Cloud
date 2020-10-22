#pragma once

namespace pdpc {

class HierarchicalGraph;
class MSSegmentation;

class MSSegmentationGraph
{
public:
    static void create(const MSSegmentation& msSegmentation, HierarchicalGraph& g);
    static void reorganize(MSSegmentation& msSegmentation, HierarchicalGraph& g);

    static bool valid(const MSSegmentation& msSegmentation, const HierarchicalGraph& g);

    // Similarities
public:
    static float jaccard(const HierarchicalGraph& hgraph, int level_edge, int idx_edge, int prop_size, int prop_weight);

}; // class MSSegmentationGraph

} // namespace pdpc
