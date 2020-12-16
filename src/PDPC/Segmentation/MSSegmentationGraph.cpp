#include <PDPC/Graph/HierarchicalGraph.h>
#include <PDPC/Segmentation/MSSegmentation.h>
#include <PDPC/Segmentation/MSSegmentationGraph.h>
#include <PDPC/Common/Assert.h>

#include <numeric>

namespace pdpc {

using String = std::string;

void MSSegmentationGraph::create(const MSSegmentation& msSegmentation, HierarchicalGraph& g)
{
    g.clear();
    g.resize_level(msSegmentation.size());

    auto prop_label  = g.add_node_property<int>("label");
    auto prop_size   = g.add_node_property<Scalar>("size");
    auto prop_id     = g.add_node_property<String>("id");
    auto prop_scale  = g.add_node_property<Scalar>("scale");
    auto prop_weight = g.add_edge_property<Scalar>("weight");

//    #pragma omp parrallel for
    for(int level=0; level<g.level_count(); ++level)
    {
        int lmax = msSegmentation[level].label_max();
        g.resize_node(level, lmax+1);

        for(int idx=0; idx<msSegmentation[level].size(); ++idx)
        {
            int label = msSegmentation[level][idx];
            if(label != Segmentation::INVALID())
            {
                g.node_property<int>(prop_label, level, label) = label;
                g.node_property<String>(prop_id, level, label) = "n_" + std::to_string(level) + "." + std::to_string(idx);
                g.node_property<Scalar>(prop_scale, level, label) = msSegmentation.scale(level);
                ++g.node_property<Scalar>(prop_size, level, label);
            }
        }
    }

//    #pragma omp parrallel for
    for(int idx=0; idx<msSegmentation[0].size(); ++idx)
    {
        for(int l=0; l<g.level_count()-1; ++l)
        {
            int k = l+1;

            int labelk = msSegmentation[k][idx];
            int labell = msSegmentation[l][idx];

            if(labell != Segmentation::INVALID() && labelk != Segmentation::INVALID())
            {
                int e = g.get_or_add_edge(l, labelk, labell);
                PDPC_DEBUG_ASSERT(e != -1);
                g.edge_property<Scalar>(prop_weight, l, e) += Scalar(1);
            }
        }
    }

    PDPC_DEBUG_ASSERT(valid(msSegmentation, g));
}

void MSSegmentationGraph::reorganize(MSSegmentation& msSegmentation, HierarchicalGraph& g)
{
    const int level_count = g.level_count();
    const int top_level   = level_count-1;

    PDPC_DEBUG_ASSERT(g.node_properties(0).has("label"));
    PDPC_DEBUG_ASSERT(g.node_properties(0).has("size"));
    PDPC_DEBUG_ASSERT(g.node_properties(0).has("id"));
    PDPC_DEBUG_ASSERT(g.edge_properties(0).has("weight"));

    int prop_label  = g.node_properties(0).index("label");
    int prop_size   = g.node_properties(0).index("size");
    int prop_id     = g.node_properties(0).index("id");
    int prop_weight = g.edge_properties(0).index("weight");

    // 1. sort top level by size -----------------------------------------------
    {
        int level = top_level;
        int label_count = g.node_count(level);

        std::vector<int> indices(label_count);
        std::iota(indices.begin(), indices.end(), 0);
        std::sort(indices.begin(), indices.end(), [&](int i, int j)->bool
        {
            return g.node_property<Scalar>(prop_size, level, i) >
                   g.node_property<Scalar>(prop_size, level, j);
        });

        std::vector<int> ranking(label_count);
        for(int i=0; i<label_count; ++i) ranking[indices[i]] = i;

        // relabeling
        for(int idx=0; idx<msSegmentation[level].size(); ++idx)
        {
            int l = msSegmentation[level].label(idx);
            if(l != Segmentation::INVALID())
                msSegmentation[level].set_label(idx, ranking[l]);
        }

        // sort
        for(int i=0; i<label_count; /*...*/)
        {
            int j = ranking[i];

            if(i==j)
            {
                ++i;
            }
            else
            {
                g.swap_node(level, i, j);
                std::swap(ranking[i], ranking[j]);
            }
        }
    }
    PDPC_DEBUG_ASSERT(valid(msSegmentation, g));
    // -------------------------------------------------------------------------
    // 2.                        -----------------------------------------------
    {
        for(int level=top_level-1; level>=0; --level)
        {
            int label_count = g.node_count(level);

            std::vector<int> ranking(label_count);
            std::iota(ranking.begin(), ranking.end(), 0);

            int processed = 0;
            for(int father_node_idx=0; father_node_idx<g.node_count(level+1); ++father_node_idx)
            {
                Node& father = g.node(level+1, father_node_idx);

                int son_count = father.out.size();

                std::vector<int> indices(son_count);
                std::iota(indices.begin(), indices.end(), 0);
                std::sort(indices.begin(), indices.end(), [&](int i, int j)->bool
                {
                    return g.edge_property<Scalar>(prop_weight, level, father.out[i]) / g.node_property<Scalar>(prop_size, level+1, g.source(level, father.out[i])) >
                           g.edge_property<Scalar>(prop_weight, level, father.out[j]) / g.node_property<Scalar>(prop_size, level+1, g.source(level, father.out[j]));
                });

                for(int k=0; k<son_count; ++k)
                {
                    int edge_idx = father.out[indices[k]];
                    Edge& edge   = g.edge(level, edge_idx);
                    int node_idx = edge.target;

                    Scalar weight = g.edge_property<Scalar>(prop_weight, level, edge_idx) / g.node_property<Scalar>(prop_size, level+1, g.source(level, edge_idx));

                    if(node_idx < processed)
                        continue;

                    bool greater_edge = false;
                    for(int other_edge_idx : g.node(level, node_idx).in)
                    {
                        Scalar other_weight = g.edge_property<Scalar>(prop_weight, level, other_edge_idx) / g.node_property<Scalar>(prop_size, level+1, other_edge_idx);
                        if(other_edge_idx != edge_idx && other_weight > weight)
                        {
                            break;
                            greater_edge = true;
                        }
                    }
                    if(greater_edge) continue;

                    g.swap_node(level, processed, node_idx);
                    std::swap(ranking[processed], ranking[node_idx]);
                    ++processed;
                }
            }

            // relabeling
            std::vector<int> indices(label_count);
            for(int i=0; i<label_count; ++i)
                indices[ranking[i]] = i;

            for(int idx=0; idx<msSegmentation[level].size(); ++idx)
            {
                int l = msSegmentation[level].label(idx);
                if(l != Segmentation::INVALID())
                    msSegmentation[level].set_label(idx, indices[l]);
            }
        }
    }
    PDPC_DEBUG_ASSERT(valid(msSegmentation, g));
    // 3 reset labels
    for(int level=0; level<level_count; ++level)
    {
        int node_count = g.node_count(level);
        for(int n=0; n<node_count; ++n)
        {
            g.node_property<int>(prop_label, level, n) = n;
            g.node_property<String>(prop_id, level, n) = "n_" + std::to_string(level) + "." + std::to_string(n);
        }
    }

    PDPC_DEBUG_ASSERT(valid(msSegmentation, g));
}

bool MSSegmentationGraph::valid(const MSSegmentation& msSegmentation, const HierarchicalGraph& g)
{
    PDPC_DEBUG_ASSERT(g.node_properties(0).has("label"));
    PDPC_DEBUG_ASSERT(g.node_properties(0).has("size"));
    PDPC_DEBUG_ASSERT(g.node_properties(0).has("id"));
    PDPC_DEBUG_ASSERT(g.edge_properties(0).has("weight"));

//    auto prop_label  = g.node_properties(0).index("label");
    auto prop_size   = g.node_properties(0).index("size");
//    auto prop_id     = g.node_properties(0).index("id");
//    auto prop_weight = g.edge_properties(0).index("weight");

    if(msSegmentation.size() != g.level_count())
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }

    for(int level=0; level<g.level_count(); ++level)
    {
        const int regionCount = msSegmentation[level].region_count();
        const int nodeCount   = g.node_count(level);
        if(regionCount != nodeCount)
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
        for(int n=0; n<g.node_count(level); ++n)
        {
            const int regionSize = g.node_property<Scalar>(prop_size, level, n);
            const int nodeSize   = msSegmentation[level].size(n);
            if(regionSize != nodeSize)
            {
                PDPC_DEBUG_ASSERT(false);
                return false;
            }
        }
    }

    return true;
}

float MSSegmentationGraph::jaccard(const HierarchicalGraph& hgraph, int level_edge, int idx_edge, int prop_size, int prop_weight)
{
    int idx_source = hgraph.source(level_edge, idx_edge);
    int idx_target = hgraph.target(level_edge, idx_edge);

    Scalar size_source = hgraph.node_property<Scalar>(prop_size,   level_edge+1, idx_source);
    Scalar size_target = hgraph.node_property<Scalar>(prop_size,   level_edge,   idx_target);
    Scalar edge_weight = hgraph.edge_property<Scalar>(prop_weight, level_edge,   idx_edge);

    Scalar index = edge_weight / (size_source + size_target - edge_weight);

    PDPC_DEBUG_ASSERT(0 <= index && index <= 1);
    return index;
}

} // namespace pdpc
