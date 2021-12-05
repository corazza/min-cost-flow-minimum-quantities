#include "network.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <stack>

void Network::add_edge(vertex_key v_from, vertex_key v_to, int cost, int capacity,
                       int minimum_quantity) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    this->costs[edge] = cost;
    this->capacities[edge] = capacity;
    this->minimum_quantities[edge] = minimum_quantity;

    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

void Network::remove_edge(vertex_key v_from, vertex_key v_to) {
    assert(!"Network::remove_edge unimplemented!");
}

bool Network::exists_edge(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    return this->costs.find(edge) != this->costs.end();
}

bool Network::exists_path(vertex_key v_from, vertex_key v_to) {
    std::set<vertex_key> visited;
    std::stack<vertex_key> to_visit;
    to_visit.push(v_from);
    while (!to_visit.empty()) {
        vertex_key visiting = to_visit.top();
        to_visit.pop();
        if (visiting == v_to) {
            return true;
        }
        visited.insert(visiting);
        for (auto v : this->outgoing[visiting]) {
            if (visited.find(v) == visited.end()) {
                to_visit.push(v);
            }
        }
    }
    return false;
}

int flow_value(Flow& f, Network& network) { assert(!"flow_value unimplemented!"); }

int flow_cost(Flow& f, Network& network) { assert(!"flow_cost unimplemented!"); }

inline edge_key get_edge_key(vertex_key v_from, vertex_key v_to) {
    return (std::uint64_t)v_from << 32 | (std::uint64_t)v_to;  // edge 64b = (v1 32b, v2 32b)
}

inline std::pair<vertex_key, vertex_key> get_vertex_keys(edge_key edge) {
    std::uint32_t v_from = (std::uint32_t)(edge >> 32);
    std::uint32_t v_to = (std::uint32_t)(edge & (((std::uint64_t)1 << 32) - 1));
    return std::make_pair((vertex_key)v_from, (vertex_key)v_to);
}
