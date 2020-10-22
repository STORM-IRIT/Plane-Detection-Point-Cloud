#include <PDPC/SpacePartitioning/KnnGraph.h>
#include <PDPC/SpacePartitioning/KdTree.h>

#include <PDPC/Common/Progress.h>

namespace pdpc {

// KnnGraph --------------------------------------------------------------------

KnnGraph::KnnGraph() :
    m_k(0),
    m_points(nullptr),
    m_indices(nullptr)
{
}

KnnGraph::KnnGraph(int k) :
    m_k(k),
    m_points(nullptr),
    m_indices(nullptr)
{
}

void KnnGraph::clear()
{
    m_points  = nullptr;
    m_indices = nullptr;
}

void KnnGraph::build(const KdTree& kdtree, bool verbose)
{
    this->clear();

    const int size = kdtree.point_count();

    m_points = kdtree.point_ptr();

    m_indices = std::make_shared<std::vector<int>>(size * m_k, -1);
    auto& indices = *m_indices.get();

    auto q = kdtree.k_nearest_index_query(m_k);

    auto prog = Progress(size, verbose);
    #pragma omp parallel for firstprivate(q)
    for(int i=0; i<size; ++i)
    {
        q.set_index(i);

        int j = 0;
        for(int n : q)
        {
            indices[i * m_k + j] = n;
            ++j;
        }
        ++prog;
    }
}

void KnnGraph::build(const KdTree& kdtree, int k, bool verbose)
{
    m_k = k;
    this->build(kdtree, verbose);
}

void KnnGraph::build(const KdTree& kdtree, const std::vector<int>& indices, bool verbose)
{
    this->clear();

    const int size = indices.size();

    m_points = kdtree.point_ptr();

    m_indices = std::make_shared<std::vector<int>>(size * m_k, -1);

    auto q = kdtree.k_nearest_index_query(m_k);

    auto prog = Progress(size, verbose);
    #pragma omp parallel for firstprivate(q)
    for(int i=0; i<size; ++i)
    {
        q.set_index(indices[i]);

        int j = 0;
        for(int n : q)
        {
            (*m_indices)[i * m_k + j] = n;
            ++j;
        }
        ++prog;
    }
}

void KnnGraph::build(const KdTree& kdtree, int k, const std::vector<int>& indices, bool verbose)
{
    m_k = k;
    this->build(kdtree, indices, verbose);
}

// Query -----------------------------------------------------------------------

KnnGraphQuery KnnGraph::k_nearest_neighbors(int index) const
{
    return KnnGraphQuery(this, index);
}

KnnGraphRangeQuery KnnGraph::range_neighbors(int index, Scalar r) const
{
    return KnnGraphRangeQuery(this, r, index);
}

int KnnGraph::k_neighbor(int idx_point, int i) const
{
    return m_indices->operator[](idx_point * m_k + i);
}

// Empty Query -----------------------------------------------------------------

KnnGraphRangeQuery KnnGraph::range_query(Scalar r) const
{
    return RangeIndexQuery(this, r);
}

// Accessors -------------------------------------------------------------------

int KnnGraph::k() const
{
    return m_k;
}

int KnnGraph::size() const
{
    return m_points->size();
}

const Vector3Array& KnnGraph::point_data() const
{
    return *m_points;
}

Vector3Array& KnnGraph::point_data()
{
    return *m_points;
}

const std::vector<int>& KnnGraph::index_data() const
{
    return *m_indices.get();
}

std::vector<int>& KnnGraph::index_data()
{
    return *m_indices.get();
}

} // namespace pdpc
