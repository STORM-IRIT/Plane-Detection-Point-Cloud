#include <PDPC/Graph/Graph.h>
#include <PDPC/Common/Assert.h>

#include <random>

namespace pdpc {

// Graph -----------------------------------------------------------------------

Graph::Graph() :
    m_nodes(0),
    m_edges(0),
    m_node_properties(0),
    m_edge_properties(0)
{
}

Graph::Graph(int node_count) :
    m_nodes(node_count),
    m_edges(0),
    m_node_properties(node_count),
    m_edge_properties(0)
{
}

Graph::Graph(int node_count, int edge_count) :
    m_nodes(node_count),
    m_edges(edge_count),
    m_node_properties(node_count),
    m_edge_properties(edge_count)
{
}

bool Graph::is_consistent() const
{
    // check property size
    if(node_properties().size() != node_count() )
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }
    if(edge_properties().size() != edge_count() )
    {
        PDPC_DEBUG_ASSERT(false);
        return false;
    }

    // check all nodes in/out edges
    for(int node=0; node<node_count(); ++node)
    {
        const Node& n = this->node(node);
        for(int edge : n.in)
        {
            if(target(edge) != node)
            {
                PDPC_DEBUG_ASSERT(false);
                return false;
            }
        }
        for(int edge : n.out)
        {
            if(source(edge) != node)
            {
                PDPC_DEBUG_ASSERT(false);
                return false;
            }
        }
    }

    // check all edges source/target
    for(int edge=0; edge<this->edge_count(); ++edge)
    {
        const Edge& e = this->edge(edge);
        auto itOut = std::find(node(e.source).out.begin(),
                               node(e.source).out.end(),
                               edge);
        if(itOut == node(e.source).out.end())
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
        auto itIn = std::find(node(e.target).in.begin(),
                              node(e.target).in.end(),
                              edge);
        if(itIn == node(e.target).out.end())
        {
            PDPC_DEBUG_ASSERT(false);
            return false;
        }
    }

    return true;
}

bool Graph::operator == (const Graph& other) const
{
    //TODO properties ?
    return (nodes() == other.nodes()) &&
           (edges() == other.edges());
}

std::stringstream& Graph::to_string(std::stringstream& str) const
{
    str << "node count = " << node_count() << "\n";
    str << "edge count = " << edge_count() << "\n";
    str << "\n";

    str << "node in:\n";
    for(int n=0; n<node_count(); ++n)
    {
        str << n << ": ";
        for(uint e=0; e<node(n).in.size(); ++e)
        {
            str << node(n).in[e] << " ";
        }
        str << "\n";
    }
    str << "\n";

    str << "node out:\n";
    for(int n=0; n<node_count(); ++n)
    {
        str << n << ": ";
        for(uint e=0; e<node(n).out.size(); ++e)
        {
            str << node(n).out[e] << " ";
        }
        str << "\n";
    }
    str << "\n";

    str << "edges :\n";
    for(int e=0; e<edge_count(); ++e)
    {
        str << e << ": " << source(e) << " -> " << target(e) << "\n";
    }
    return str;
}

// Modifiers -------------------------------------------------------------------

void Graph::clear()
{
    nodes().clear();
    edges().clear();
    node_properties().clear();
    edge_properties().clear();
}

int Graph::add_node()
{
    nodes().emplace_back();
    node_properties().emplace_back();
    return node_count()-1;
}

int Graph::add_edge(int source, int target)
{
    return add_edge(Edge{source, target});
}

int Graph::add_edge(const Edge& e)
{
    PDPC_DEBUG_ASSERT(this->contains_node(e.source));
    PDPC_DEBUG_ASSERT(this->contains_node(e.target));
    edges().emplace_back(e);
    edge_properties().emplace_back();
    int edge = edge_count()-1;
    nodes()[e.source].out.push_back(edge);
    nodes()[e.target].in.push_back(edge);
    return edge;
}

void Graph::remove_node(int node)
{
    isolate(node);

    for(Edge& e : edges())
    {
        PDPC_DEBUG_ASSERT(e.source!=node);
        PDPC_DEBUG_ASSERT(e.target!=node);
        if(e.source > node) --e.source;
        if(e.target > node) --e.target;
    }
    nodes().erase(nodes().begin()+node);
    PDPC_DEBUG_ASSERT(false); //TODO
//    node_properties().erase(node);
}

void Graph::remove_edge(int edge)
{
    edges().erase(edges().begin()+edge);
    for(Node& n : nodes())
    {
        for(int i=0; i<int(n.in.size());)
        {
            if(n.in[i]==edge)
            {
                n.in.erase(n.in.begin()+i);
            }
            else if(n.in[i]>edge)
            {
                --(n.in[i]);
                ++i;
            }
            else
            {
                ++i;
            }
        }
        for(int i=0; i<int(n.out.size());)
        {
            if(n.out[i]==edge)
            {
                n.out.erase(n.out.begin()+i);
            }
            else if(n.out[i]>edge)
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
    PDPC_DEBUG_ASSERT(false); //TODO
//    edge_properties().erase(edge);
}

void Graph::remove_in_edges(int node)
{
    while(!this->node(node).in.empty())
        remove_edge(this->node(node).in.front());
}


void Graph::remove_out_edges(int node)
{
    while(!this->node(node).out.empty())
        remove_edge(this->node(node).out.front());
}

void Graph::isolate(int node)
{
    remove_in_edges(node);
    remove_out_edges(node);
    PDPC_DEBUG_ASSERT(this->is_isolated(node));
}

void Graph::set_random(int node_count)
{
    clear();
    resize_node(node_count);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(0.5);

    for(int source=0; source<node_count; ++source)
    {
        for(int target=0; target<node_count; ++target)
        {
            if(d(gen))
            {
                add_edge(source, target);
            }
        }
    }
}


} // namespace pdpc
