#pragma once

#include <PDPC/Graph/Edge.h>
#include <PDPC/Graph/Node.h>

#include <PDPC/Common/Containers/PropertyMap.h>

namespace pdpc {

//!
//! \brief The Graph class represents a directed graph.
//!
//! Each node is referenced by a unique id n.
//! Each edge is referenced by a unique id e.
//!
//! An edge is directed from a source node to a target node. It only refers to
//! its source and target nodes ids.
//!
//! A node is connected by in edges and out edges. It refers to a list of in
//! edges ids, and a list of out edges ids. These lists are not sorted (TODO ?).
//!
class Graph
{
public:
    using NodeArray         = std::vector<Node>;
    using EdgeArray         = std::vector<Edge>;
    using NodePropertyMap   = property_map;
    using EdgePropertyMap   = property_map;

    // Graph -------------------------------------------------------------------
public:
    Graph();
    Graph(int node_count);
    Graph(int node_count, int edge_count);

    bool is_consistent() const;
    bool operator == (const Graph& other) const;
    std::stringstream& to_string(std::stringstream& str) const;

    // Capacity ----------------------------------------------------------------
public:
    inline bool empty() const;

    inline int node_count() const;
    inline int edge_count() const;

    inline void resize_node(int size);
    inline void reserve_node(int size);

    inline void resize_edge(int size);
    inline void reserve_edge(int size);

    // Modifiers ---------------------------------------------------------------
public:
    void clear();

    int add_node();
    int add_edge(int source, int target);
    int add_edge(const Edge& e);

    void remove_node(int node);
    void remove_edge(int edge);

    void remove_in_edges(int node);
    void remove_out_edges(int node);

    void isolate(int node);

    void set_random(int node_count);

    // Node Accessors ----------------------------------------------------------
public:
    inline const Node& node(int node) const;
    inline       Node& node(int node);

    inline bool contains_node(int node) const;

    inline const NodeArray& nodes() const;
    inline       NodeArray& nodes();

    inline bool is_isolated(int node) const;

    inline int degree(int node) const;
    inline int degree_in(int node) const;
    inline int degree_out(int node) const;

    // Node Property -----------------------------------------------------------
public:
    template<typename P>
    inline int add_node_property(const std::string& name);

    template<typename P>
    inline void remove_node_property(const std::string& name);

    template<typename P>
    inline void remove_node_property(int prop);

    template<typename P>
    inline const P& node_property(int prop, int node) const;

    template<typename P>
    inline P& node_property(int prop, int node);

    template<typename P>
    inline const P& node_property(const std::string& name, int node) const;

    template<typename P>
    inline P& node_property(const std::string& name, int node);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& node_property(int prop) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& node_property(int prop);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& node_property(const std::string& name) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& node_property(const std::string& name);

    inline const NodePropertyMap& node_properties() const;
    inline       NodePropertyMap& node_properties();

    // Edge Accessors ----------------------------------------------------------
public:
    inline int  source(int edge) const;
    inline int& source(int edge);

    inline int  target(int edge) const;
    inline int& target(int edge);

    inline int edge(int source, int target) const;

    inline bool contains_edge(int source, int target) const;
    inline bool contains_edge(int edge) const;

    inline const Edge& edge(int edge) const;
    inline       Edge& edge(int edge);

    inline const EdgeArray& edges() const;
    inline       EdgeArray& edges();

    // Edge Property -----------------------------------------------------------
public:
    template<typename P>
    inline int add_edge_property(const std::string& name);

    template<typename P>
    inline void remove_edge_property(const std::string& name);

    template<typename P>
    inline void remove_edge_property(int prop);

    template<typename P>
    inline const P& edge_property(int prop, int edge) const;

    template<typename P>
    inline P& edge_property(int prop, int edge);

    template<typename P>
    inline const P& edge_property(const std::string& name, int edge) const;

    template<typename P>
    inline P& edge_property(const std::string& name, int edge);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& edge_property(int prop) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& edge_property(int prop);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& edge_property(const std::string& name) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& edge_property(const std::string& name);

    inline const EdgePropertyMap& edge_properties() const;
    inline       EdgePropertyMap& edge_properties();

    // Data --------------------------------------------------------------------
protected:
    NodeArray       m_nodes;
    EdgeArray       m_edges;
    NodePropertyMap m_node_properties;
    EdgePropertyMap m_edge_properties;

}; // class Graph

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Capacity --------------------------------------------------------------------

bool Graph::empty() const
{
    return node_count() == 0;
}

int Graph::node_count() const
{
    return nodes().size();
}

int Graph::edge_count() const
{
    return edges().size();
}

void Graph::resize_node(int size)
{
    nodes().resize(size);
    node_properties().resize(size);
}

void Graph::reserve_node(int size)
{
    nodes().reserve(size);
    node_properties().reserve(size);
}

void Graph::resize_edge(int size)
{
    edges().resize(size);
    edge_properties().resize(size);
}

void Graph::reserve_edge(int size)
{
    edges().reserve(size);
    edge_properties().reserve(size);
}

// Node Accessors --------------------------------------------------------------

const Node& Graph::node(int node) const
{
    return nodes()[node];
}

Node& Graph::node(int node)
{
    return nodes()[node];
}

bool Graph::contains_node(int node) const
{
    return node < node_count();
}

const typename Graph::NodeArray& Graph::nodes() const
{
    return m_nodes;
}

typename Graph::NodeArray& Graph::nodes()
{
    return m_nodes;
}

bool Graph::is_isolated(int node) const
{
    return degree(node) == 0;
}

int Graph::degree(int node) const
{
    return degree_in(node) + degree_out(node);
}

int Graph::degree_in(int node) const
{
    return this->node(node).in.size();
}

int Graph::degree_out(int node) const
{
    return this->node(node).out.size();
}

// Node Property ---------------------------------------------------------------

template<typename P>
int Graph::add_node_property(const std::string& name)
{
    return node_properties().add<P>(name);
}

template<typename P>
void Graph::remove_node_property(const std::string& name)
{
    node_properties().discard<P>(name);
}

template<typename P>
void Graph::remove_node_property(int prop)
{
    node_properties().discard<P>(prop);
}

template<typename P>
const P& Graph::node_property(int prop, int node) const
{
    return node_properties().at<P>(prop, node);
}

template<typename P>
P& Graph::node_property(int prop, int node)
{
    return node_properties().at<P>(prop, node);
}

template<typename P>
const P& Graph::node_property(const std::string& name, int node) const
{
    return node_properties().at<P>(name, node);
}

template<typename P>
P& Graph::node_property(const std::string& name, int node)
{
    return node_properties().at<P>(name, node);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& Graph::node_property(int prop) const
{
    return node_properties().at<P>(prop);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& Graph::node_property(int prop)
{
    return node_properties().at<P>(prop);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& Graph::node_property(const std::string& name) const
{
    return node_properties().at<P>(name);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& Graph::node_property(const std::string& name)
{
    return node_properties().at<P>(name);
}

const typename Graph::NodePropertyMap& Graph::node_properties() const
{
    return m_node_properties;
}

typename Graph::NodePropertyMap& Graph::node_properties()
{
    return m_node_properties;
}

// Edge Accessors --------------------------------------------------------------

int Graph::source(int edge) const
{
    return this->edge(edge).source;
}

int& Graph::source(int edge)
{
    return this->edge(edge).source;
}

int Graph::target(int edge) const
{
    return this->edge(edge).target;
}

int& Graph::target(int edge)
{
    return this->edge(edge).target;
}

int Graph::edge(int source, int target) const
{
    auto it = std::find_if(node(source).out.begin(), node(source).out.end(), [&](int e)->bool
    {
        return edge(e).target == target;
    });
    return std::distance(node(source).out.begin(), it);
}

bool Graph::contains_edge(int source, int target) const
{
    auto it = std::find_if(node(source).out.begin(), node(source).out.end(), [&](int e)->bool
    {
        return edge(e).target == target;
    });
    return it != node(source).out.end() && contains_edge(*it);
}

bool Graph::contains_edge(int edge) const
{
    return edge < edge_count();
}

const Edge& Graph::edge(int edge) const
{
    return edges()[edge];
}

Edge& Graph::edge(int edge)
{
    return edges()[edge];
}

const typename Graph::EdgeArray& Graph::edges() const
{
    return m_edges;
}

typename Graph::EdgeArray& Graph::edges()
{
    return m_edges;
}

// Edge Property ---------------------------------------------------------------

template<typename P>
int Graph::add_edge_property(const std::string& name)
{
    return edge_properties().add<P>(name);
}

template<typename P>
void Graph::remove_edge_property(const std::string& name)
{
    return edge_properties().add<P>(name);
}

template<typename P>
void Graph::remove_edge_property(int prop)
{
    edge_properties().discard<P>(prop);
}

template<typename P>
const P& Graph::edge_property(int prop, int edge) const
{
    return edge_properties().at<P>(prop, edge);
}

template<typename P>
P& Graph::edge_property(int prop, int edge)
{
    return edge_properties().at<P>(prop, edge);
}

template<typename P>
const P& Graph::edge_property(const std::string& name, int edge) const
{
    return edge_properties().at<P>(name, edge);
}

template<typename P>
P& Graph::edge_property(const std::string& name, int edge)
{
    return edge_properties().at<P>(name, edge);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& Graph::edge_property(int prop) const
{
    return edge_properties().at<P>(prop);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& Graph::edge_property(int prop)
{
    return edge_properties().at<P>(prop);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& Graph::edge_property(const std::string& name) const
{
    return edge_properties().at<P>(name);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& Graph::edge_property(const std::string& name)
{
    return edge_properties().at<P>(name);
}

const typename Graph::EdgePropertyMap& Graph::edge_properties() const
{
    return m_edge_properties;
}

typename Graph::EdgePropertyMap& Graph::edge_properties()
{
    return m_edge_properties;
}

} // namespace pdpc
