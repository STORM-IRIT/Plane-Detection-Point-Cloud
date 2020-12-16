#include <PDPC/SpacePartitioning/KdTree.h>

#include <numeric>

namespace pdpc {

// KdTree ----------------------------------------------------------------------

KdTree::KdTree() :
    m_points(nullptr),
    m_nodes(nullptr),
    m_indices(nullptr),
    m_min_cell_size(64)
{
}

KdTree::KdTree(std::shared_ptr<Vector3Array>& points) :
    m_points(nullptr),
    m_nodes(nullptr),
    m_indices(nullptr),
    m_min_cell_size(64)
{
    this->build(points);
}

KdTree::KdTree(std::shared_ptr<Vector3Array>& points, const std::vector<int>& sampling) :
    m_points(nullptr),
    m_nodes(nullptr),
    m_indices(nullptr),
    m_min_cell_size(64)
{
    this->build(points, sampling);
}

void KdTree::clear()
{
    m_points  = nullptr;
    m_nodes   = nullptr;
    m_indices = nullptr;
}

bool KdTree::valid() const
{
    if(m_points == nullptr)
        return m_nodes == nullptr && m_indices == nullptr;

    if(m_nodes == nullptr || m_indices == nullptr)
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }

    if(m_points->size() < m_indices->size())
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }

    std::vector<bool> b(m_points->size(), false);
    for(int idx : *m_indices.get())
    {
        if(idx < 0 || int(m_points->size()) <= idx || b[idx])
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
        b[idx] = true;
    }

    for(uint n=0; n<m_nodes->size(); ++n)
    {
        const KdTreeNode& node = m_nodes->operator [](n);
        if(node.leaf)
        {
            if(m_indices->size() <= node.start || m_indices->size() < node.start+node.size)
            {
                PDPC_DEBUG_ASSERT(false);
                return false;
            }
        }
        else
        {
            if(node.dim < 0 || 2 < node.dim)
            {
                PDPC_DEBUG_ASSERT(false);
                return false;
            }
            if(m_nodes->size() <= node.firstChildId || m_nodes->size() <= node.firstChildId+1u)
            {
                PDPC_DEBUG_ASSERT(false);
                return false;
            }
        }
    }

    return true;
}

std::string KdTree::to_string() const
{
    if(!m_indices) return "";

    std::stringstream str;
    str << "indices (" << m_indices->size() << ") :\n";
    for(uint i=0; i<m_indices->size(); ++i)
    {
        str << "  " << i << ": " << m_indices->operator[](i) << "\n";
    }
    str << "nodes (" << m_nodes->size() << ") :\n";
    for(uint n=0; n<m_nodes->size(); ++n)
    {
        const KdTreeNode& node = m_nodes->operator[](n);
        if(node.leaf)
        {
            int end = node.start + node.size;
            str << "  leaf: start=" << node.start << " end=" << end << " (size=" << node.size << ")\n";
        }
        else
        {
            str << "  node: dim=" << node.dim << " split=" << node.splitValue << " child=" << node.firstChildId << "\n";
        }
    }
    return str.str();
}

void KdTree::build(std::shared_ptr<Vector3Array>& points)
{
    this->clear();

    m_points = points;

    m_nodes = std::make_shared<std::vector<KdTreeNode>>();
    m_nodes->reserve(4 * m_points->size() / m_min_cell_size);
    m_nodes->emplace_back();
    m_nodes->back().leaf = false;

    m_indices = std::make_shared<std::vector<int>>(m_points->size());
    std::iota(m_indices->begin(), m_indices->end(), 0);

    this->build_rec(0, 0, m_points->size(), 1);

    PDPC_DEBUG_ASSERT(this->valid());
}

void KdTree::build(std::shared_ptr<Vector3Array>& points, const std::vector<int>& sampling)
{
    this->clear();

    m_points = points;

    m_nodes = std::make_shared<std::vector<KdTreeNode>>();
    m_nodes->reserve(4 * m_points->size() / m_min_cell_size);
    m_nodes->emplace_back();
    m_nodes->back().leaf = false;

    m_indices = std::make_shared<std::vector<int>>(sampling);

    this->build_rec(0, 0, m_indices->size(), 1);

    PDPC_DEBUG_ASSERT(this->valid());
}

void KdTree::rebuild(const std::vector<int>& sampling)
{
    PDPC_DEBUG_ASSERT(sampling.size() <= m_points->size());

    m_nodes->clear();
    m_nodes->emplace_back();
    m_nodes->back().leaf = false;

    *m_indices = sampling;

    this->build_rec(0, 0, m_indices->size(), 1);

    PDPC_DEBUG_ASSERT(this->valid());
}

// Query -----------------------------------------------------------------------

KdTreeKNearestPointQuery KdTree::k_nearest_neighbors(const Vector3& point, int k) const
{
    return KNearestPointQuery(this, k, point);
}

KdTreeKNearestIndexQuery KdTree::k_nearest_neighbors(int index, int k) const
{
    return KNearestIndexQuery(this, k, index);
}

KdTreeNearestPointQuery KdTree::nearest_neighbor(const Vector3& point) const
{
    return NearestPointQuery(this, point);
}

KdTreeNearestIndexQuery KdTree::nearest_neighbor(int index) const
{
    return NearestIndexQuery(this, index);
}

KdTreeRangePointQuery KdTree::range_neighbors(const Vector3& point, Scalar r) const
{
    return RangePointQuery(this, r, point);
}

KdTreeRangeIndexQuery KdTree::range_neighbors(int index, Scalar r) const
{
    return RangeIndexQuery(this, r, index);
}

// Empty Query -----------------------------------------------------------------

KdTreeKNearestPointQuery KdTree::k_nearest_point_query(int k) const
{
    return KNearestPointQuery(this, k);
}

KdTreeKNearestIndexQuery KdTree::k_nearest_index_query(int k) const
{
    return KNearestIndexQuery(this, k);
}

KdTreeNearestPointQuery KdTree::nearest_point_query() const
{
    return NearestPointQuery(this);
}

KdTreeNearestIndexQuery KdTree::nearest_index_query() const
{
    return NearestIndexQuery(this);
}

KdTreeRangePointQuery KdTree::range_point_query(Scalar r) const
{
    return RangePointQuery(this, r);
}

KdTreeRangeIndexQuery KdTree::range_index_query(Scalar r) const
{
    return RangeIndexQuery(this, r);
}

// Accessors -------------------------------------------------------------------

int KdTree::node_count() const
{
    return m_nodes->size();
}

int KdTree::index_count() const
{
    return m_indices->size();
}

int KdTree::point_count() const
{
    return m_points->size();
}

const Vector3Array& KdTree::point_data() const
{
    return *m_points.get();
}

Vector3Array& KdTree::point_data()
{
    return *m_points.get();
}

const std::shared_ptr<Vector3Array>& KdTree::point_ptr() const
{
    return m_points;
}

std::shared_ptr<Vector3Array>& KdTree::point_ptr()
{
    return m_points;
}

const std::vector<KdTreeNode>& KdTree::node_data() const
{
    return *m_nodes.get();
}

std::vector<KdTreeNode>& KdTree::node_data()
{
    return *m_nodes.get();
}

const std::vector<int>& KdTree::index_data() const
{
    return *m_indices.get();
}

std::vector<int>& KdTree::index_data()
{
    return *m_indices.get();
}

// Parameters ------------------------------------------------------------------

int KdTree::min_cell_size() const
{
    return m_min_cell_size;
}

void KdTree::set_min_cell_size(int min_cell_size)
{
    m_min_cell_size = min_cell_size;
}

// Internal --------------------------------------------------------------------

void KdTree::build_rec(int node_id, int start, int end, int level)
{
    auto& nodes = *m_nodes.get();
    const auto& points  = *m_points.get();
    const auto& indices = *m_indices.get();

    KdTreeNode& node = nodes[node_id];
    Aabb aabb;
    for(int i=start; i<end; ++i)
        aabb.extend(points[indices[i]]);

    Vector3 diag = Scalar(0.5)*(aabb.max()-aabb.min());
    int dim;
    diag.maxCoeff(&dim);

    node.dim = dim;
    node.splitValue = aabb.center()(dim);

    int midId = this->partition(start, end, dim, node.splitValue);
    node.firstChildId = nodes.size();

    {
        KdTreeNode n;
        n.size = 0;
        nodes.push_back(n);
        nodes.push_back(n);
    }
    {
        // left child
        int childId = nodes[node_id].firstChildId;
        KdTreeNode& child = nodes[childId];
        if(midId-start <= m_min_cell_size || level >= PDPC_KDTREE_MAX_DEPTH)
        {
            child.leaf = 1;
            child.start = start;
            child.size = midId-start;
        }
        else
        {
            child.leaf = 0;
            this->build_rec(childId, start, midId, level+1);
        }
    }
    {
        // right child
        int childId = nodes[node_id].firstChildId+1;
        KdTreeNode& child = nodes[childId];
        if(end-midId <= m_min_cell_size || level >= PDPC_KDTREE_MAX_DEPTH)
        {
            child.leaf = 1;
            child.start = midId;
            child.size = end-midId;
        }
        else
        {
            child.leaf = 0;
            this->build_rec(childId, midId, end, level+1);
        }
    }
}

int KdTree::partition(int start, int end, int dim, Scalar value)
{
    const auto& points = *m_points.get();
    auto& indices  = *m_indices.get();

    auto it = std::partition(indices.begin()+start, indices.begin()+end, [&](int i)
    {
        return points[i][dim] < value;
    });
    return std::distance(m_indices->begin(), it);
}

} // namespace pdpc
