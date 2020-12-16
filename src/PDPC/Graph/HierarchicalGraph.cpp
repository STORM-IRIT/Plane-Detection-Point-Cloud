#include <PDPC/Graph/HierarchicalGraph.h>
#include <PDPC/Graph/Graph.h>

namespace pdpc {

// HierarchicalGraph -----------------------------------------------------------

HierarchicalGraph::HierarchicalGraph() :
    m_nodes(),
    m_edges(),
    m_node_properties(),
    m_edge_properties()
{
}

HierarchicalGraph::HierarchicalGraph(int level_count) :
    m_nodes(level_count),
    m_edges(level_count-1),
    m_node_properties(level_count),
    m_edge_properties(level_count-1)
{
}

bool HierarchicalGraph::is_consistent() const
{
    if(!m_nodes.empty() && m_edges.size() != m_nodes.size()-1)
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    if(m_nodes.size() != m_node_properties.size())
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    if(m_edges.size() != m_edge_properties.size())
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    for(uint i=0; i<m_node_properties.size()-1; ++i)
    {
        if(!m_node_properties[i]._is_valid() ||
           m_node_properties[i].size() != int(m_nodes[i].size()))
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    for(uint i=0; i<m_edge_properties.size()-1; ++i)
    {
        if(!m_edge_properties[i]._is_valid() ||
           m_edge_properties[i].size() != int(m_edges[i].size()))
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    for(uint i=1; i<m_node_properties.size()-1; ++i)
    {
        if(!m_node_properties[0].match(m_node_properties[i]))
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    for(uint i=1; i<m_edge_properties.size()-1; ++i)
    {
        if(!m_edge_properties[0].match(m_edge_properties[i]))
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }
    return true;
}

std::stringstream& HierarchicalGraph::to_string(std::stringstream& str) const
{
    str << "level count = " << level_count() << "\n";
    str << "node count  = " << node_count() << "\n";
    str << "edge count  = " << edge_count() << "\n";
    str << "\n";

    for(int level=level_count()-1; level>=0; --level)
    {
        str << "level: " << level << " node count = " << node_count(level) << "\n";
        if(level > 0)
        {
            str << "\tmid level: " << level-1 << " edge count = " << edge_count(level-1) << "\n";
        }
    }
    str << "\n";

    for(int level=level_count()-1; level>=0; --level)
    {
        str << "level: " << level << "\n";
        str << "\tnode count = " << node_count(level) << "\n";
        str << "\tnode in:\n";
        for(int n=0; n<node_count(level); ++n)
        {
            str << "\t" << n << ": ";
            for(uint e=0; e<node(level, n).in.size(); ++e)
            {
                str << node(level, n).in[e] << " ";
            }
            str << "\n";
        }
        str << "\tnode out:\n";
        for(int n=0; n<node_count(level); ++n)
        {
            str << "\t" << n << ": ";
            for(uint e=0; e<node(level, n).out.size(); ++e)
            {
                str << node(level, n).out[e] << " ";
            }
            str << "\n";
        }
        if(level > 0)
        {
            str << "mid level: " << level-1 << "\n";
            str << "\tedge count = " << edge_count(level-1) << "\n";
            str << "\tedges :\n";
            for(int e=0; e<edge_count(level-1); ++e)
            {
                str << "\t" << e << ": " << source(level-1, e) << " -> " << target(level-1, e) << "\n";
            }
        }
    }
    return str;
}

Graph& HierarchicalGraph::to_graph(Graph& g, bool add_prop) const
{
    g.clear();
    g.node_properties().discard_all();
    g.edge_properties().discard_all();

    // initialize with first level of node
    g.nodes()           = this->nodes(0);
    g.node_properties() = this->node_properties(0);
    g.edge_properties().transfer_from(this->edge_properties(0));

    int n_prev_begin = 0;
    int n_prev_end   = g.node_count();
    int e_prev_end   = 0;

    for(int l=1; l<this->level_count(); ++l)
    {
        int n_next_begin = n_prev_end;
        int n_next_end   = n_next_begin + this->node_count(l);

        int e_next_begin = e_prev_end;
        int e_next_end   = e_next_begin + this->edge_count(l-1);

        g.nodes().insert(g.nodes().end(), this->nodes(l  ).begin(), this->nodes(l  ).end());
        g.edges().insert(g.edges().end(), this->edges(l-1).begin(), this->edges(l-1).end());
        g.node_properties().insert(g.node_properties().end(), this->node_properties(l  ).begin(), this->node_properties(l  ).end());
        g.edge_properties().insert(g.edge_properties().end(), this->edge_properties(l-1).begin(), this->edge_properties(l-1).end());

        for(int n=n_prev_begin; n<n_prev_end; ++n)
        {
            for(int& e : g.node(n).in)
            {
                e += e_next_begin;
            }
        }
        for(int n=n_next_begin; n<n_next_end; ++n)
        {
            for(int& e : g.node(n).out)
            {
                e += e_next_begin;
            }
        }
        for(int e=e_next_begin; e<e_next_end; ++e)
        {
            g.edge(e).source += n_next_begin;
            g.edge(e).target += n_prev_begin;
        }

        n_prev_begin = n_next_begin;
        n_prev_end   = n_next_end;
        e_prev_end   = e_next_end;
    }

    if(add_prop)
    {
        int p = g.add_node_property<int>("level");
        int accu = 0;

        for(int l=0; l<this->level_count(); ++l)
        {
            int count = this->node_count(l);
            std::fill(g.node_property<int>(p).begin() + accu,
                      g.node_property<int>(p).begin() + accu + count, l);
            accu += count;
        }
    }

    return g;
}

HierarchicalGraph& HierarchicalGraph::from_graph(const Graph& g, bool add_prop)
{
    this->clear(); // clear => discard all properties ! TODO make this clear

    PDPC_DEBUG_ASSERT(g.node_properties().has("level"));
    int prop_level = g.node_properties().index("level");

    // nodes -------------------------------------------------------------------
    std::vector<int> node_begin_vec;
    std::vector<int> node_end_vec;

    int node_begin = 0;
    int node_end   = 0;
    for(int level=0; node_end<g.node_count(); ++level)
    {
        node_begin = node_end;

        while(node_end < g.node_count() && g.node_property<int>(prop_level, node_end)==level)
            ++node_end;

        node_begin_vec.push_back(node_begin);
        node_end_vec.push_back(node_end);

        this->add_level();
        this->top_nodes().assign(g.nodes().begin() + node_begin, g.nodes().begin() + node_end);

        if(add_prop)
        {
            this->top_node_properties().transfer_from(g.node_properties());
            this->top_node_properties().assign(g.node_properties().begin() + node_begin, g.node_properties().begin() + node_end);
        }
    }

    // edges -------------------------------------------------------------------
    std::vector<int> edge_begin_vec;
    std::vector<int> edge_end_vec;

    int edge_begin = 0;
    int edge_end   = 0;
    for(int level=0; edge_end<g.edge_count(); ++level)
    {
        edge_begin = edge_end;

        //TODO since the old MSSegmentationGraph used to save hierarchical graph
        //     with unoriented edges going from a low source to a high target
        //     which is the inverse of this HierarchicalGraph class, source and
        //     target are swapped here (see also next TODO)
        //                                                                    vvvvvv
        while(edge_end < g.edge_count() && g.node_property<int>(prop_level, g.target(edge_end))==level)
            ++edge_end;

        edge_begin_vec.push_back(edge_begin);
        edge_end_vec.push_back(edge_end);

        this->edges(level).assign(g.edges().begin() + edge_begin, g.edges().begin() + edge_end);
        if(add_prop)
        {
            this->edge_properties(level).transfer_from(g.edge_properties());
            this->edge_properties(level).assign(g.edge_properties().begin()+edge_begin, g.edge_properties().begin()+edge_end);
        }

        //TODO swap source/target (cf previous TODO)
//        for(int e=0; e<this->edge_count(level); ++e)
//            std::swap(source(level, e), target(level, e));
    }

    PDPC_DEBUG_ASSERT(edge_begin_vec.size() == node_begin_vec.size()-1);

    for(int level=0; level<this->level_count(); ++level)
    {
        for(int n=0; n<this->node_count(level); ++n)
        {
            if(level < this->level_count()-1)
            {
                for(int& e : this->node(level, n).in)
                {
                    e -= edge_begin_vec[level];
                }
            }
            if(level > 0)
            {
                for(int& e : this->node(level, n).out)
                {
                    e -= edge_begin_vec[level-1];
                }
            }
        }
    }

    for(int level=0; level<this->level_count()-1; ++level)
    {
        for(int e=0; e<this->edge_count(level); ++e)
        {
            this->source(level, e) -= node_begin_vec[level+1];
            this->target(level, e) -= node_begin_vec[level];
        }
    }

    PDPC_DEBUG_ASSERT(g.node_count() == this->node_count());
    PDPC_DEBUG_ASSERT(g.edge_count() == this->edge_count());
    PDPC_DEBUG_ASSERT(this->is_consistent());
    return *this;
}

int HierarchicalGraph::node_index(int level, int n) const
{
    int sum = 0;
    for(int l=0; l<level; ++l)
    {
        sum += node_count(l);
    }
    return sum + n;
}

int HierarchicalGraph::node_index(const PairIndex& idx) const
{
    return node_index(idx.first, idx.second);
}

HierarchicalGraph::PairIndex HierarchicalGraph::node_indices(int n) const
{
    int level = 0;
    int sum = node_count(0);
    while(sum <= n)
    {
        ++level;
        sum += node_count(level);
    }
    int node = n - (sum - node_count(level));
    return std::make_pair(level, node);
}

int HierarchicalGraph::edge_index(int level, int e) const
{
    int sum = 0;
    for(int l=0; l<level; ++l)
    {
        sum += edge_count(l);
    }
    return sum + e;
}

int HierarchicalGraph::edge_index(const PairIndex& idx) const
{
    return edge_index(idx.first, idx.second);
}

HierarchicalGraph::PairIndex HierarchicalGraph::edge_indices(int e) const
{
    int level = 0;
    int sum = edge_count(0);
    while(sum <= e)
    {
        ++level;
        sum += edge_count(level);
    }
    int edge = e - (sum - edge_count(level));
    return std::make_pair(level, edge);
}

// Modifiers -------------------------------------------------------------------

void HierarchicalGraph::clear()
{
    nodes().clear();
    edges().clear();
    node_properties().clear();
    edge_properties().clear();
}

void HierarchicalGraph::add_level()
{
    int old_level_count = level_count();
    nodes().emplace_back();
    node_properties().emplace_back();
    if(old_level_count > 0)
    {
        edges().emplace_back();
        edge_properties().emplace_back();
    }
    //TODO transfert properties to the new level ?
    //TODO keep always one node_properties level to keep trace of prop ?
}

int HierarchicalGraph::add_node(int level)
{
    nodes(level).emplace_back();
    node_properties(level).emplace_back();
    return nodes(level).size()-1;
}

int HierarchicalGraph::add_edge(int mid_level, int source, int target)
{
    return add_edge(mid_level, {source, target});
}

int HierarchicalGraph::add_edge(int mid_level, const Edge& e)
{
    edges(mid_level).emplace_back(e);
    edge_properties(mid_level).emplace_back();
    int edge = edges(mid_level).size()-1;
    node(mid_level+1, e.source).out.push_back(edge);
    node(mid_level  , e.target).in.push_back(edge);
    return edge;
}

void HierarchicalGraph::remove_node(int level, int node)
{
    PDPC_DEBUG_ASSERT(contains_node(level, node));

    isolate(level, node);

    nodes(level).erase(nodes(level).begin()+node);
    node_properties(level).erase(node);

    if(level < level_count()-1)
    {
        for(Edge& e : edges(level))
        {
            if(e.source > node) --e.source;
            if(e.target > node) --e.target;
        }
    }

    if(level > 0)
    {
        for(Edge& e : edges(level-1))
        {
            if(e.source > node) --e.source;
            if(e.target > node) --e.target;
        }
    }
}

void HierarchicalGraph::remove_edge(int level, int edge)
{
    PDPC_DEBUG_ASSERT(contains_edge(level, edge));

    edges(level).erase(edges(level).begin()+edge);
    edge_properties(level).erase(edge);

    for(Node& n : nodes(level+1))
    {
        for(int i=0; i<int(n.out.size()); ++i)
        {
            if(n.out[i] == edge)
            {
                n.out.erase(n.out.begin()+i);
            }
            else if(n.out[i] > edge)
            {
                --(n.out[i]);
                ++i;
            }
            else
            {
                ++i;
            }
        }
    }

    for(Node& node : nodes(level))
    {
        for(int i=0; i<int(node.in.size()); ++i)
        {
            if(node.in[i] == edge)
            {
                node.in.erase(node.in.begin()+i);
            }
            else if(node.in[i] > edge)
            {
                --(node.in[i]);
                ++i;
            }
            else
            {
                ++i;
            }
        }
    }
}

void HierarchicalGraph::remove_in_edges(int level, int node)
{
    PDPC_DEBUG_ASSERT(contains_node(level, node));

    while(!this->node(level, node).in.empty())
        remove_edge(level, this->node(level, node).in.front());
}

void HierarchicalGraph::remove_out_edges(int level, int node)
{
    PDPC_DEBUG_ASSERT(contains_node(level, node));

    while(!this->node(level, node).out.empty())
        remove_edge(level-1, this->node(level, node).out.front());
}

void HierarchicalGraph::isolate(int level, int node)
{
    PDPC_DEBUG_ASSERT(contains_node(level, node));

    remove_in_edges(level, node);
    remove_out_edges(level, node);
    PDPC_DEBUG_ASSERT(this->is_isolated(level, node));
}

void HierarchicalGraph::swap_node(int level, int node1, int node2)
{
    PDPC_DEBUG_ASSERT(contains_node(level, node1));
    PDPC_DEBUG_ASSERT(contains_node(level, node2));

    if(node1 == node2) return;

    std::swap(node(level, node1), node(level, node2));
    node_properties(level).swap(node1, node2);

    if(level > 0)
    {
        for(Edge& edge : edges(level-1))
        {
            if(     edge.source == node1) edge.source = node2;
            else if(edge.source == node2) edge.source = node1;
        }
    }
    if(level<level_count()-1)
    {
        for(Edge& edge : edges(level))
        {
            if(     edge.target == node1) edge.target = node2;
            else if(edge.target == node2) edge.target = node1;
        }
    }
}

} // namespace pdpc
