#include <PDPC/SpacePartitioning/KdTree/Query/KdTreeNearestIndexQuery.h>
#include <PDPC/SpacePartitioning/KdTree.h>

namespace pdpc {

KdTreeNearestIndexQuery::KdTreeNearestIndexQuery() :
    KdTreeQuery(),
    NearestIndexQuery()
{
}

KdTreeNearestIndexQuery::KdTreeNearestIndexQuery(const KdTree* kdtree) :
    KdTreeQuery(kdtree),
    NearestIndexQuery()
{
}

KdTreeNearestIndexQuery::KdTreeNearestIndexQuery(const KdTree* kdtree, int index) :
    KdTreeQuery(kdtree),
    NearestIndexQuery(index)
{
}

KdTreeNearestIndexIterator KdTreeNearestIndexQuery::begin()
{
    this->search();
    return KdTreeNearestIndexIterator(m_nearest);
}

KdTreeNearestIndexIterator KdTreeNearestIndexQuery::end()
{
    return KdTreeNearestIndexIterator(m_nearest+1);
}

const NearestIndexQuery& KdTreeNearestIndexQuery::search()
{
    const auto& nodes   = m_kdtree->node_data();
    const auto& points  = m_kdtree->point_data();
    const auto& indices = m_kdtree->index_data();
    const auto& point   = points[m_index];

    m_stack.clear();
    m_stack.push({0,0});

    m_nearest = m_index==indices[0] ? indices[1] : indices[0];
    m_squared_distance = (point - points[m_nearest]).squaredNorm();

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
                    if(m_index == idx) continue;

                    Scalar d = (point - points[idx]).squaredNorm();
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
                Scalar newOff = point[node.dim] - node.splitValue;
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
    return *this;
}

} // namespace pdpc
