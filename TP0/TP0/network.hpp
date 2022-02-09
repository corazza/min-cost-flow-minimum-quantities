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
    unsigned int n_nodes;
    vertex_key source;
    vertex_key sink;

    std::unordered_map<vertex_key, std::set<vertex_key> > outgoing;
    std::unordered_map<vertex_key, std::set<vertex_key> > incoming;

    std::unordered_map<edge_key, int> costs;
    std::unordered_map<edge_key, int> capacities;          // upper bound of edge key
    std::unordered_map<edge_key, int> minimum_quantities;  // lower bound of edge_key
    std::set<edge_key> vlbs;  // true iff edge_key has a variable lower bound

    void add_edge(vertex_key v_from, vertex_key v_to, int cost, int capacity, int minimum_quantity,
                  bool vlb);

    void remove_edge(vertex_key v_from, vertex_key v_to);

    int capacity(vertex_key v_from, vertex_key v_to) const;

    bool exists_edge(vertex_key v_from, vertex_key v_to);  // considers direction

    bool exists_path(vertex_key v_from, vertex_key v_to) const;

    bool respects_lower_bounds(const Flow &flow) const;
    bool respects_upper_bounds(const Flow &flow) const;
    bool respects_bounds(const Flow &flow) const;

    std::set<edge_key> detect_wannabe_active_vlbs(const Flow &flow) const;

    unsigned int n_edges() {
        return this->costs.size();
    }

    unsigned int n_outgoing(vertex_key v);

    Network(unsigned int n_nodes) : n_nodes(n_nodes), source(0), sink(n_nodes-1) {}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Network, n_nodes, source, sink, outgoing, incoming, costs, capacities, minimum_quantities, vlbs);

#endif

