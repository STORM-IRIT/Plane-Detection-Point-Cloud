#pragma once

#include <PDPC/SpacePartitioning/KnnGraph/Query/KnnGraphQuery.h>
#include <PDPC/SpacePartitioning/KnnGraph/Query/KnnGraphRangeQuery.h>

#include <memory>

namespace pdpc {

class KdTree;

class KnnGraph
{
    // Types -------------------------------------------------------------------
public:
    using KNearestIndexQuery = KnnGraphQuery;
    using RangeIndexQuery    = KnnGraphRangeQuery;

    // KnnGraph ----------------------------------------------------------------
public:
    KnnGraph();
    KnnGraph(int k);

    void clear();
    void build(const KdTree& kdtree, bool verbose = false);
    void build(const KdTree& kdtree, int k, bool verbose = false);
    void build(const KdTree& kdtree, const std::vector<int>& indices, bool verbose = false);
    void build(const KdTree& kdtree, int k, const std::vector<int>& indices, bool verbose = false);


    // Query -------------------------------------------------------------------
public:
    KNearestIndexQuery k_nearest_neighbors(int index) const;
    RangeIndexQuery    range_neighbors(int index, Scalar r) const;

    int k_neighbor(int index, int i) const;

    // Empty Query -------------------------------------------------------------
public:
    RangeIndexQuery range_query(Scalar r = 0) const;

    // Accessors ---------------------------------------------------------------
public:
    int k() const;
    int size() const;

    const Vector3Array& point_data() const;
          Vector3Array& point_data();

    const std::vector<int>& index_data() const;
          std::vector<int>& index_data();

    // Data --------------------------------------------------------------------
protected:
    int m_k;
    std::shared_ptr<Vector3Array>     m_points;
    std::shared_ptr<std::vector<int>> m_indices;
};

} // namespace pdpc
