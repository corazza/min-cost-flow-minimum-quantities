/* - representation of MCNF-MQ problem network
 *    - deserialize from JSON
 *    - serialize to JSON
 * - representation of flows
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <set>
#include <unordered_map>

#include "json.hpp"
using json = nlohmann::json;

// needed because C++ doesn't have default implementations for pair<int, int> keys in a hashmap ...
typedef std::uint32_t vertex_key;
typedef std::uint64_t edge_key;
// ... so this hack is the best solution (https://stackoverflow.com/a/39690912/924313)
inline edge_key get_edge_key(vertex_key v_from, vertex_key v_to);
inline std::pair<vertex_key, vertex_key> get_vertex_keys(edge_key edge);

struct Network {
    unsigned int n_nodes;
    vertex_key source;
    vertex_key sink;
    unsigned int flow_value;  // _desired_ flow value

    std::unordered_map<vertex_key, std::set<vertex_key> > outgoing;
    std::unordered_map<vertex_key, std::set<vertex_key> > incoming;

    std::unordered_map<edge_key, int> costs;
    std::unordered_map<edge_key, int> capacities;
    std::unordered_map<edge_key, int> minimum_quantities;

    bool exists_edge(vertex_key v_from, vertex_key v_to);  // considers direction
    bool exists_path(vertex_key v_from, vertex_key v_to);

    void add_edge(vertex_key v_from, vertex_key v_to, int cost, int capacity, int minimum_quantity);
    void remove_edge(vertex_key v_from, vertex_key v_to);

    Network() {}
    Network(unsigned int n_nodes, unsigned int flow_value)
        : n_nodes(n_nodes), source(0), sink(1), flow_value(flow_value) {}
};

// ignore (utility for json serialization)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Network, n_nodes, source, sink, flow_value, costs, capacities,
                                   minimum_quantities, outgoing, incoming);

typedef std::unordered_map<edge_key, int> Flow;

// assumes f respects flow conservation (sum inputs = sum outputs on each node, etc.)
// returns -1 if:
//  - f doesn't match network (f contains edge that doesn't exist in network)
//  - f doesn't respect variable lower bounds or upper bounds (capacities)
// returns the sum of inputs to the sink node otherwise
int flow_value(Flow& f, Network& network);

int flow_cost(Flow& f, Network& network);

#endif
