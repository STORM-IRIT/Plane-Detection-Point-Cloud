#include <PDPC/Segmentation/SeededKNNGraphRegionGrowing.h>

namespace pdpc {

template<class CompFuncT, class PriorityCompFunc, class InitFuncT>
void SeededKNNGraphRegionGrowing::compute(const PointCloud& point_cloud,
                                          Segmentation& segmentation,
                                          CompFuncT&& comp_f,
                                          PriorityCompFunc&& priority_f,
                                          InitFuncT&& init_f,
                                          bool verbose)
{
    PDPC_DEBUG_ASSERT(point_cloud.has_knn_graph());

    // init segmentation
    const int size = point_cloud.size();
    segmentation.resize(size);
    segmentation.reset(Segmentation::invalid());

    // init priority queue
    std::vector<int> queue(size);
    std::iota(queue.begin(), queue.end(), 0);
    std::sort(queue.begin(), queue.end(), priority_f);

    // init stack
    std::vector<int> stack;

    auto prog = Progress(size, verbose);

    while(!queue.empty())
    {
        // get new seed
        const int idx_seed = queue.back();
        queue.pop_back();

        if(segmentation[idx_seed] == Segmentation::invalid())
        {
            // init new region
            const int new_label = segmentation.new_label();
            segmentation.set_label(idx_seed, new_label);

            init_f(new_label, idx_seed);
            stack.push_back(idx_seed);

            // growing
            while(!stack.empty())
            {
                int idx_point = stack.back();
                stack.pop_back();

                for(int idx_nei : point_cloud.knn_graph().k_nearest_neighbors(idx_point))
                {
                    if(segmentation[idx_nei] == Segmentation::invalid())
                    {
                        if(comp_f(new_label, idx_point, idx_nei))
                        {
                            segmentation.set_label(idx_nei, new_label);
                            stack.push_back(idx_nei);
                        }
                    }
                }
            }
        }
        ++prog;
    }
}

} // namespace pdpc
