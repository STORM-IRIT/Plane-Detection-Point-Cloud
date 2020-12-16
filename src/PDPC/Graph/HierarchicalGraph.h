#pragma once

#include <PDPC/Graph/Edge.h>
#include <PDPC/Graph/Node.h>

#include <PDPC/Common/Containers/PropertyMap.h>

namespace pdpc {

class Graph;

class HierarchicalGraph
{
    // Types -------------------------------------------------------------------
public:
    using NodeArray           = std::vector<Node>;
    using EdgeArray           = std::vector<Edge>;
    using NodePropertyMap     = property_map;
    using EdgePropertyMap     = property_map;
    using PairIndex           = std::pair<int,int>;

    // HierarchicalGraph -------------------------------------------------------
public:
    HierarchicalGraph();
    HierarchicalGraph(int level_count);

    bool is_consistent() const;
    std::stringstream& to_string(std::stringstream& str) const;

    // Graph -------------------------------------------------------------------
public:
    Graph& to_graph(Graph& g, bool add_prop = true) const;
    HierarchicalGraph& from_graph(const Graph& g, bool add_prop = true);

    int node_index(int level, int n) const;
    int node_index(const PairIndex& idx) const;
    PairIndex node_indices(int n) const;

    int edge_index(int level, int e) const;
    int edge_index(const PairIndex& idx) const;
    PairIndex edge_indices(int e) const;

    // Capacity ----------------------------------------------------------------
public:
    inline bool empty() const;

    inline int node_count() const;
    inline int edge_count() const;
    inline int node_count(int level) const;
    inline int edge_count(int mid_level) const;

    inline int level_count() const;
    inline int mid_level_count() const;

    inline void resize_level(int level_count);

    inline void resize_node(int size);
    inline void reserve_node(int size);
    inline void resize_node(int level, int size);
    inline void reserve_node(int level, int size);

    inline void resize_edge(int size);
    inline void reserve_edge(int size);
    inline void resize_edge(int mid_level, int size);
    inline void reserve_edge(int mid_level, int size);

    // Modifiers ---------------------------------------------------------------
public:
    void clear();

    void add_level();

    int add_node(int level);
    int add_edge(int mid_level, int source, int target);
    int add_edge(int mid_level, const Edge& e);

    void remove_node(int level, int node);
    void remove_edge(int level, int edge);

    void remove_in_edges(int level, int node);
    void remove_out_edges(int level, int node);

    void isolate(int level, int node);

    void swap_node(int level, int node1, int node2);

    // Node Accessors ----------------------------------------------------------
public:
    inline const Node& node(int level, int node) const;
    inline       Node& node(int level, int node);

    inline bool contains_node(int level, int node) const;

    inline const NodeArray& top_nodes() const;
    inline       NodeArray& top_nodes();

    inline const NodeArray& bottom_nodes() const;
    inline       NodeArray& bottom_nodes();

    inline const NodeArray& nodes(int level) const;
    inline       NodeArray& nodes(int level);

    inline const std::vector<NodeArray>& nodes() const;
    inline       std::vector<NodeArray>& nodes();

    inline bool is_isolated(int level, int node) const;

    inline int degree(int level, int node) const;
    inline int degree_in(int level, int node) const;
    inline int degree_out(int level, int node) const;

    // Node Property -----------------------------------------------------------
public:
    template<typename P>
    inline int add_node_property(const std::string& name);

    template<typename P>
    inline void remove_node_property(const std::string& name);

    template<typename P>
    inline void remove_node_property(int prop);

    template<typename P>
    inline const P& node_property(int prop, int level, int node) const;

    template<typename P>
    inline P& node_property(int prop, int level, int node);

    template<typename P>
    inline const P& node_property(const std::string& name, int level, int node) const;

    template<typename P>
    inline P& node_property(const std::string& name, int level, int node);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& node_property(int prop, int level) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& node_property(int prop, int level);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& node_property(const std::string& name, int level) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& node_property(const std::string& name, int level);

    inline const NodePropertyMap& node_properties(int level) const;
    inline       NodePropertyMap& node_properties(int level);

    inline const NodePropertyMap& top_node_properties() const;
    inline       NodePropertyMap& top_node_properties();

    inline const std::vector<NodePropertyMap>& node_properties() const;
    inline       std::vector<NodePropertyMap>& node_properties();

    // Edge Accessors ----------------------------------------------------------
public:
    inline int  source(int mid_level, int edge) const;
    inline int& source(int mid_level, int edge);

    inline int  target(int mid_level, int edge) const;
    inline int& target(int mid_level, int edge);

    inline bool contains_edge(int level, int edge) const;

    inline int get_edge(int mid_level, int source, int target) const;
    inline int get_or_add_edge(int mid_level, int source, int target);

    inline const Edge& edge(int mid_level, int edge) const;
    inline       Edge& edge(int mid_level, int edge);

    inline const EdgeArray& edges(int mid_level) const;
    inline       EdgeArray& edges(int mid_level);

    inline const std::vector<EdgeArray>& edges() const;
    inline       std::vector<EdgeArray>& edges();

    // Edge Property -----------------------------------------------------------
public:
    template<typename P>
    inline int add_edge_property(const std::string& name);

    template<typename P>
    inline void remove_edge_property(const std::string& name);

    template<typename P>
    inline void remove_edge_property(int prop);

    template<typename P>
    inline const P& edge_property(int prop, int mid_level, int edge) const;

    template<typename P>
    inline P& edge_property(int prop, int mid_level, int edge);

    template<typename P>
    inline const P& edge_property(const std::string& name, int mid_level, int edge) const;

    template<typename P>
    inline P& edge_property(const std::string& name, int mid_level, int edge);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& edge_property(int prop, int mid_level) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& edge_property(int prop, int mid_level);

    template<typename P>
    inline const std::vector<P, typename allocator<P>::type>& edge_property(const std::string& name, int mid_level) const;

    template<typename P>
    inline std::vector<P, typename allocator<P>::type>& edge_property(const std::string& name, int mid_level);

    inline const EdgePropertyMap& edge_properties(int mid_level) const;
    inline       EdgePropertyMap& edge_properties(int mid_level);

    inline const std::vector<EdgePropertyMap>& edge_properties() const;
    inline       std::vector<EdgePropertyMap>& edge_properties();

    // Data --------------------------------------------------------------------
protected:
    std::vector<NodeArray>       m_nodes;
    std::vector<EdgeArray>       m_edges;
    std::vector<NodePropertyMap> m_node_properties;
    std::vector<EdgePropertyMap> m_edge_properties;

}; // class HierarchicalGraph

////////////////////////////////////////////////////////////////////////////////

// Capacity --------------------------------------------------------------------

bool HierarchicalGraph::empty() const
{
    return node_count() == 0;
}

int HierarchicalGraph::node_count() const
{
    int count = 0;
    for(int l=0; l<level_count(); ++l)
    {
        count += node_count(l);
    }
    return count;
}

int HierarchicalGraph::edge_count() const
{
    int count = 0;
    for(int l=0; l<mid_level_count(); ++l)
    {
        count += edge_count(l);
    }
    return count;
}

int HierarchicalGraph::node_count(int level) const
{
    return nodes(level).size();
}

int HierarchicalGraph::edge_count(int mid_level) const
{
    return edges(mid_level).size();
}

int HierarchicalGraph::level_count() const
{
    return nodes().size();
}

int HierarchicalGraph::mid_level_count() const
{
    return edges().size();
}

void HierarchicalGraph::resize_level(int level_count)
{
    nodes().resize(level_count);
    edges().resize(level_count-1);
    node_properties().resize(level_count);
    edge_properties().resize(level_count-1);
}

void HierarchicalGraph::resize_node(int size)
{
    for(int l=0; l<level_count(); ++l)
    {
        resize_node(l, size);
    }
}

void HierarchicalGraph::reserve_node(int size)
{
    for(int l=0; l<level_count(); ++l)
    {
        reserve_node(l, size);
    }
}

void HierarchicalGraph::resize_node(int level, int size)
{
    nodes(level).resize(size);
    node_properties(level).resize(size);
}

void HierarchicalGraph::reserve_node(int level, int size)
{
    nodes(level).reserve(size);
    node_properties(level).reserve(size);
}

void HierarchicalGraph::resize_edge(int size)
{
    for(int l=0; l<mid_level_count(); ++l)
    {
        resize_edge(l, size);
    }
}

void HierarchicalGraph::reserve_edge(int size)
{
    for(int l=0; l<mid_level_count(); ++l)
    {
        reserve_edge(l, size);
    }
}

void HierarchicalGraph::resize_edge(int mid_level, int size)
{
    edges(mid_level).resize(size);
    edge_properties(mid_level).resize(size);
}

void HierarchicalGraph::reserve_edge(int mid_level, int size)
{
    edges(mid_level).reserve(size);
    edge_properties(mid_level).reserve(size);
}

// Node Accessors --------------------------------------------------------------

const Node& HierarchicalGraph::node(int level, int node) const
{
    return nodes(level)[node];
}

Node& HierarchicalGraph::node(int level, int node)
{
    return nodes(level)[node];
}

bool HierarchicalGraph::contains_node(int level, int node) const
{
    return node < node_count(level);
}

const typename HierarchicalGraph::NodeArray& HierarchicalGraph::top_nodes() const
{
    return nodes(level_count()-1);
}

typename HierarchicalGraph::NodeArray& HierarchicalGraph::top_nodes()
{
    return nodes(level_count()-1);
}

const typename HierarchicalGraph::NodeArray& HierarchicalGraph::bottom_nodes() const
{
    return nodes(0);
}

typename HierarchicalGraph::NodeArray& HierarchicalGraph::bottom_nodes()
{
    return nodes(0);
}

const typename HierarchicalGraph::NodeArray& HierarchicalGraph::nodes(int level) const
{
    return nodes()[level];
}

typename HierarchicalGraph::NodeArray& HierarchicalGraph::nodes(int level)
{
    return nodes()[level];
}

const std::vector<typename HierarchicalGraph::NodeArray>& HierarchicalGraph::nodes() const
{
    return m_nodes;
}

std::vector<typename HierarchicalGraph::NodeArray>& HierarchicalGraph::nodes()
{
    return m_nodes;
}

bool HierarchicalGraph::is_isolated(int level, int node) const
{
    return degree(level, node) == 0;
}

int HierarchicalGraph::degree(int level, int node) const
{
    return degree_in(level, node) + degree_out(level, node);
}

int HierarchicalGraph::degree_in(int level, int node) const
{
    return this->node(level, node).in.size();
}

int HierarchicalGraph::degree_out(int level, int node) const
{
    return this->node(level, node).out.size();
}

// Node Property ---------------------------------------------------------------

template<typename P>
int HierarchicalGraph::add_node_property(const std::string& name)
{
    int idx = -1;
    for(int l=0; l<level_count(); ++l)
    {
        idx = node_properties(l).add<P>(name);
    }
    return idx;
}

template<typename P>
void HierarchicalGraph::remove_node_property(const std::string& name)
{
    for(int l=0; l<level_count(); ++l)
    {
        node_properties(l).template discard<P>(name);
    }
}

template<typename P>
void HierarchicalGraph::remove_node_property(int prop)
{
    for(int l=0; l<level_count(); ++l)
    {
        node_properties(l).template discard<P>(prop);
    }
}

template<typename P>
const P& HierarchicalGraph::node_property(int prop, int level, int node) const
{
    return node_properties(level).at<P>(prop, node);
}

template<typename P>
P& HierarchicalGraph::node_property(int prop, int level, int node)
{
    return node_properties(level).at<P>(prop, node);
}

template<typename P>
const P& HierarchicalGraph::node_property(const std::string& name, int level, int node) const
{
    return node_properties(level).at<P>(name, node);
}

template<typename P>
P& HierarchicalGraph::node_property(const std::string& name, int level, int node)
{
    return node_properties(level).at<P>(name, node);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& HierarchicalGraph::node_property(int prop, int level) const
{
    return node_properties(level).at<P>(prop);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& HierarchicalGraph::node_property(int prop, int level)
{
    return node_properties(level).at<P>(prop);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& HierarchicalGraph::node_property(const std::string& name, int level) const
{
    return node_properties(level).at<P>(name);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& HierarchicalGraph::node_property(const std::string& name, int level)
{
    return node_properties(level).at<P>(name);
}

const typename HierarchicalGraph::NodePropertyMap& HierarchicalGraph::node_properties(int level) const
{
    return node_properties()[level];
}

typename HierarchicalGraph::NodePropertyMap& HierarchicalGraph::node_properties(int level)
{
    return node_properties()[level];
}

const typename HierarchicalGraph::NodePropertyMap& HierarchicalGraph::top_node_properties() const
{
    return node_properties(level_count()-1);
}

typename HierarchicalGraph::NodePropertyMap& HierarchicalGraph::top_node_properties()
{
    return node_properties(level_count()-1);
}

const std::vector<typename HierarchicalGraph::NodePropertyMap>& HierarchicalGraph::node_properties() const
{
    return m_node_properties;
}

std::vector<typename HierarchicalGraph::NodePropertyMap>& HierarchicalGraph::node_properties()
{
    return m_node_properties;
}

// Edge Accessors --------------------------------------------------------------

int HierarchicalGraph::source(int mid_level, int edge) const
{
    return this->edge(mid_level, edge).source;
}

int& HierarchicalGraph::source(int mid_level, int edge)
{
    return this->edge(mid_level, edge).source;
}

int HierarchicalGraph::target(int mid_level, int edge) const
{
    return this->edge(mid_level, edge).target;
}

int& HierarchicalGraph::target(int mid_level, int edge)
{
    return this->edge(mid_level, edge).target;
}

bool HierarchicalGraph::contains_edge(int level, int edge) const
{
    return edge < edge_count(level);
}

int HierarchicalGraph::get_edge(int mid_level, int source, int target) const
{
    if(!contains_node(mid_level+1, source) || !contains_node(mid_level, target)) return -1;

    auto begin = node(mid_level+1, source).out.begin();
    auto end   = node(mid_level+1, source).out.end();

    auto it = std::find_if(begin, end, [&](int e)->bool
    {
        return this->edge(mid_level, e).target == target;
    });

    return it == end ? -1 : *it;
}

int HierarchicalGraph::get_or_add_edge(int mid_level, int source, int target)
{
    if(!contains_node(mid_level+1, source) || !contains_node(mid_level, target)) return -1;

    auto begin = node(mid_level+1, source).out.begin();
    auto end   = node(mid_level+1, source).out.end();

    auto it = std::find_if(begin, end, [&](int e)->bool
    {
        return this->edge(mid_level, e).target == target;
    });

    return it == end ? add_edge(mid_level, source, target) : *it;
}

const Edge& HierarchicalGraph::edge(int mid_level, int edge) const
{
    PDPC_DEBUG_ASSERT(contains_edge(mid_level, edge));
    return edges(mid_level)[edge];
}

Edge& HierarchicalGraph::edge(int mid_level, int edge)
{
    PDPC_DEBUG_ASSERT(contains_edge(mid_level, edge));
    return edges(mid_level)[edge];
}

const typename HierarchicalGraph::EdgeArray& HierarchicalGraph::edges(int mid_level) const
{
    PDPC_DEBUG_ASSERT(mid_level < mid_level_count());
    return edges()[mid_level];
}

typename HierarchicalGraph::EdgeArray& HierarchicalGraph::edges(int mid_level)
{
    PDPC_DEBUG_ASSERT(mid_level < mid_level_count());
    return edges()[mid_level];
}

const std::vector<typename HierarchicalGraph::EdgeArray>& HierarchicalGraph::edges() const
{
    return m_edges;
}

std::vector<typename HierarchicalGraph::EdgeArray>& HierarchicalGraph::edges()
{
    return m_edges;
}

// Edge Property ---------------------------------------------------------------

template<typename P>
int HierarchicalGraph::add_edge_property(const std::string& name)
{
    int idx = -1;
    for(int l=0; l<mid_level_count(); ++l)
    {
        idx = edge_properties(l).add<P>(name);
    }
    return idx;
}

template<typename P>
void HierarchicalGraph::remove_edge_property(const std::string& name)
{
    for(int l=0; l<mid_level_count(); ++l)
    {
        edge_properties(l).template discard<P>(name);
    }
}

template<typename P>
void HierarchicalGraph::remove_edge_property(int prop)
{
    for(int l=0; l<mid_level_count(); ++l)
    {
        edge_properties(l).template discard<P>(prop);
    }
}

template<typename P>
const P& HierarchicalGraph::edge_property(int prop, int mid_level, int edge) const
{
    return edge_properties(mid_level).at<P>(prop, edge);
}

template<typename P>
P& HierarchicalGraph::edge_property(int prop, int mid_level, int edge)
{
    return edge_properties(mid_level).at<P>(prop, edge);
}

template<typename P>
const P& HierarchicalGraph::edge_property(const std::string& name, int mid_level, int edge) const
{
    return edge_properties(mid_level).at<P>(name, edge);
}

template<typename P>
P& HierarchicalGraph::edge_property(const std::string& name, int mid_level, int edge)
{
    return edge_properties(mid_level).at<P>(name, edge);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& HierarchicalGraph::edge_property(int prop, int mid_level) const
{
    return edge_properties(mid_level).at<P>(prop);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& HierarchicalGraph::edge_property(int prop, int mid_level)
{
    return edge_properties(mid_level).at<P>(prop);
}

template<typename P>
const std::vector<P, typename allocator<P>::type>& HierarchicalGraph::edge_property(const std::string& name, int mid_level) const
{
    return edge_properties(mid_level).at<P>(name);
}

template<typename P>
std::vector<P, typename allocator<P>::type>& HierarchicalGraph::edge_property(const std::string& name, int mid_level)
{
    return edge_properties(mid_level).at<P>(name);
}

const typename HierarchicalGraph::EdgePropertyMap& HierarchicalGraph::edge_properties(int mid_level) const
{
    return edge_properties()[mid_level];
}

typename HierarchicalGraph::EdgePropertyMap& HierarchicalGraph::edge_properties(int mid_level)
{
    return edge_properties()[mid_level];
}

const std::vector<typename HierarchicalGraph::EdgePropertyMap>& HierarchicalGraph::edge_properties() const
{
    return m_edge_properties;
}

std::vector<typename HierarchicalGraph::EdgePropertyMap>& HierarchicalGraph::edge_properties()
{
    return m_edge_properties;
}

} // namespace pdpc
