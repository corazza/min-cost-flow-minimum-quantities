/* - representation of MCNF-MQ problem network
 *    - deserialize from JSON
 *    - serialize to JSON
 * - representation of flows
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "flow.hpp"

#include <set>
#include <unordered_map>

#include "json.hpp"
#include "keys.hpp"
using json = nlohmann::json;

struct Network {

};

struct FlowNetwork {
    unsigned int n_nodes;
    vertex_key source;
    vertex_key sink;
    unsigned int flow_value;  // _desired_ flow value
    unsigned int max_span_q;

    std::unordered_map<vertex_key, std::set<vertex_key> > outgoing;
    std::unordered_map<vertex_key, std::set<vertex_key> > incoming;

    std::unordered_map<edge_key, int> costs;
    std::unordered_map<edge_key, int> capacities;          // upper bound of edge key
    std::unordered_map<edge_key, int> minimum_quantities;  // lower bound of edge_key
    std::unordered_map<edge_key, bool> vlb;  // true iff edge_key has a variable lower bound

    std::unordered_map<vertex_key, int> vertex_effective_capacity;
    std::unordered_map<edge_key, int> edge_effective_capacity;
    bool computed_effective_capacities;

    void add_edge(vertex_key v_from, vertex_key v_to, int cost, int capacity, int minimum_quantity,
                  bool vlb);

    void remove_edge(vertex_key v_from, vertex_key v_to);

    bool respects_bounds(Flow &flow) const;

    void compute_effective_capacities();

    int effective_capacity(vertex_key v);
    int effective_capacity(vertex_key v_from, vertex_key v_to);

    int capacity(vertex_key v_from, vertex_key v_to);

    bool exists_edge(vertex_key v_from, vertex_key v_to);  // considers direction

    bool exists_path(vertex_key v_from, vertex_key v_to);

    unsigned int n_edges() {
        return this->costs.size();
    }

    unsigned int n_outgoing(vertex_key v);

    FlowNetwork() : computed_effective_capacities(false) {}
    FlowNetwork(unsigned int n_nodes, unsigned int flow_value, unsigned int max_span_q)
        : n_nodes(n_nodes), source(0), sink(n_nodes-1), flow_value(flow_value), computed_effective_capacities(false), max_span_q(max_span_q) {}
};

// ignore (utility for json serialization)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FlowNetwork, n_nodes, source, sink, flow_value, costs, capacities,
                                   minimum_quantities, vlb, outgoing, incoming);


#endif

// layered_residual_network
