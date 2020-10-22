#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeNearestPointQuery.h>
#include <PDPC/SpacePartitioning/KdTree.h>

namespace pdpc {

KdTreeNearestPointQuery::KdTreeNearestPointQuery() :
    KdTreeQuery(),
    NearestPointQuery()
{
}

KdTreeNearestPointQuery::KdTreeNearestPointQuery(const KdTree* kdtree) :
    KdTreeQuery(kdtree),
    NearestPointQuery()
{
}

KdTreeNearestPointQuery::KdTreeNearestPointQuery(const KdTree* kdtree, const Vector3& point) :
    KdTreeQuery(kdtree),
    NearestPointQuery(point)
{
}

KdTreeNearestPointIterator KdTreeNearestPointQuery::begin()
{
    this->search();
    return KdTreeNearestPointIterator(m_nearest);
}

KdTreeNearestPointIterator KdTreeNearestPointQuery::end()
{
    return KdTreeNearestPointIterator(m_nearest+1);
}

void KdTreeNearestPointQuery::search()
{
    const auto& nodes   = m_kdtree->node_data();
    const auto& points  = m_kdtree->point_data();
    const auto& indices = m_kdtree->index_data();

    m_stack.clear();
    m_stack.push({0,0});

    m_nearest = indices[0];
    m_squared_distance = (m_point - points[m_nearest]).squaredNorm();

    while(!m_stack.empty())
    {
        auto& qnode = m_stack.top();
        const auto& node  = nodes[qnode.index];

        if(qnode.squared_distance < m_squared_distance)
        {
            if(node.leaf)
            {
                m_stack.pop();
                int end = node.start + node.size;
                for(int i=node.start; i<end; ++i)
                {
                    int idx = indices[i];

                    Scalar d = (m_point - points[idx]).squaredNorm();
                    if(d < m_squared_distance)
                    {
                        m_nearest = idx;
                        m_squared_distance = d;
                    }
                }
            }
            else
            {
                // replace the stack top by the farthest and push the closest
                Scalar newOff = m_point[node.dim] - node.splitValue;
                m_stack.push();
                if(newOff < 0)
                {
                    m_stack.top().index = node.firstChildId;
                    qnode.index         = node.firstChildId+1;
                }
                else
                {
                    m_stack.top().index = node.firstChildId+1;
                    qnode.index         = node.firstChildId;
                }
                m_stack.top().squared_distance = qnode.squared_distance;
                qnode.squared_distance         = newOff*newOff;
            }
        }
        else
        {
            m_stack.pop();
        }
    }
}

} // namespace pdpc
