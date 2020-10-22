#pragma once

#include <PDPC/Segmentation/Segmentation.h>

#include <PDPC/PointCloud/PointCloud.h>

#include <PDPC/SpacePartitioning/KnnGraph.h>

#include <PDPC/Common/Progress.h>

#include <numeric>

namespace pdpc {

class SeededKNNGraphRegionGrowing
{
public:

    //!
    //! \param comp_f      (int l, int i, int j) -> bool  return true if region l can grow from element i to j
    //! \param priority_f  (int i, int j)        -> bool  return true if element i has priority on element j (for seed selection)
    //! \param init_f      (int l, int i)        -> void  called when a new region l is created at element i
    //!
    template<class CompFuncT, class PriorityCompFunc, class InitFuncT>
    static void compute(const PointCloud& point_cloud,
                        Segmentation& segmentation,
                        CompFuncT&& comp_f,
                        PriorityCompFunc&& priority_f,
                        InitFuncT&& init_f,
                        bool verbose = false);
};

} // namespace pdpc

#include <PDPC/Segmentation/SeededKNNGraphRegionGrowing.hpp>
