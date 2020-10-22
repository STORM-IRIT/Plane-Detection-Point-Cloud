#pragma once

#include <PDPC/SpacePartitioning/KdTree/KdTreeNode.h>

#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeKNearestIndexQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeKNearestPointQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeNearestIndexQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeNearestPointQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeRangeIndexQuery.h>
#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeRangePointQuery.h>

#include <memory>

#define PDPC_KDTREE_MAX_DEPTH 32

namespace pdpc {

class KdTree
{
    // Types -------------------------------------------------------------------
public:
    using KNearestPointQuery = KdTreeKNearestPointQuery;
    using KNearestIndexQuery = KdTreeKNearestIndexQuery;
    using NearestPointQuery  = KdTreeNearestPointQuery;
    using NearestIndexQuery  = KdTreeNearestIndexQuery;
    using RangePointQuery    = KdTreeRangePointQuery;
    using RangeIndexQuery    = KdTreeRangeIndexQuery;

    // KdTree ------------------------------------------------------------------
public:
    KdTree();
    KdTree(std::shared_ptr<Vector3Array>& points);
    KdTree(std::shared_ptr<Vector3Array>& points, const std::vector<int>& sampling);

    void clear();
    void build(std::shared_ptr<Vector3Array>& points);
    void build(std::shared_ptr<Vector3Array>& points, const std::vector<int>& sampling);
    void rebuild(const std::vector<int>& sampling);

    bool valid() const;
    std::string to_string() const;

    // Query -------------------------------------------------------------------
public:
    KNearestPointQuery k_nearest_neighbors(const Vector3& point, int k) const;
    KNearestIndexQuery k_nearest_neighbors(int index, int k) const;
    NearestPointQuery  nearest_neighbor(const Vector3& point) const;
    NearestIndexQuery  nearest_neighbor(int index) const;
    RangePointQuery    range_neighbors(const Vector3& point, Scalar r) const;
    RangeIndexQuery    range_neighbors(int index, Scalar r) const;

    // Empty Query -------------------------------------------------------------
public:
    KNearestPointQuery k_nearest_point_query(int k = 0) const;
    KNearestIndexQuery k_nearest_index_query(int k = 0) const;
    NearestPointQuery  nearest_point_query() const;
    NearestIndexQuery  nearest_index_query() const;
    RangePointQuery    range_point_query(Scalar r = 0) const;
    RangeIndexQuery    range_index_query(Scalar r = 0) const;

    // Accessors ---------------------------------------------------------------
public:
    int node_count() const;
    int index_count() const;
    int point_count() const;

    const Vector3Array& point_data() const;
          Vector3Array& point_data();

    const std::shared_ptr<Vector3Array>& point_ptr() const;
          std::shared_ptr<Vector3Array>& point_ptr();

    const std::vector<KdTreeNode>& node_data() const;
          std::vector<KdTreeNode>& node_data();

    const std::vector<int>& index_data() const;
          std::vector<int>& index_data();

    // Parameters --------------------------------------------------------------
public:
    int min_cell_size() const;
    void set_min_cell_size(int min_cell_size);

    // Internal ----------------------------------------------------------------
public:
    void build_rec(int node_id, int start, int end, int level);
    int partition(int start, int end, int dim, Scalar value);

    // Data --------------------------------------------------------------------
protected:
    std::shared_ptr<Vector3Array>            m_points;
    std::shared_ptr<std::vector<KdTreeNode>> m_nodes;
    std::shared_ptr<std::vector<int>>        m_indices;

    int m_min_cell_size;
};

} // namespace pdpc
