#include "network.hpp"
#include "util.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <stack>
#include <queue>

void Network::add_edge(vertex_key v_from, vertex_key v_to, int cost, int capacity,
                       int minimum_quantity, bool vlb) {
    assert(v_from != v_to);
    assert(!vectorized);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    this->costs[edge] = cost;
    this->capacities[edge] = capacity;
    this->minimum_quantities[edge] = minimum_quantity;
    if (vlb) {
        this->vlbs.insert(edge);
    }

    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

void Network::remove_edge(vertex_key v_from, vertex_key v_to) {
    assert(!"FlowNetwork::remove_edge unimplemented!");
    assert(!vectorized);
}

int Network::capacity(vertex_key v_from, vertex_key v_to) const {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->capacities.find(edge) == this->capacities.end()) {
        return 0;
    } else {
        return this->capacities.at(edge);
    }
}

int Network::cost(vertex_key v_from, vertex_key v_to) const {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->costs.find(edge) == this->costs.end()) {
        return 0;
    } else {
        return this->costs.at(edge);
    }
}

int Network::minimum_quantity(vertex_key v_from, vertex_key v_to) const {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (minimum_quantities.find(edge) == minimum_quantities.end()) {
        return 0;
    } else {
        return minimum_quantities.at(edge);
    }
}

void Network::vectorize() {
    v_costs.resize(n_nodes, std::vector<int>(n_nodes, 0));
    v_capacities.resize(n_nodes, std::vector<int>(n_nodes, 0));
    v_minimum_quantities.resize(n_nodes, std::vector<int>(n_nodes, 0));
    v_vlbs.resize(n_nodes, std::vector<bool>(n_nodes, false));

    for (int i = 0; i < n_nodes; ++i) {
        int k = min(i+max_span_q, n_nodes);
        for (int j = i+1; j < k; ++j) {
            edge_key edge = get_edge_key(i, j);
            v_costs[i][j] = cost(i, j);
            v_capacities[i][j] = capacity(i, j);
            v_minimum_quantities[i][j] = minimum_quantity(i, j);
        }
    }

    for (auto edge : vlbs) {
        auto vertices = get_vertex_keys(edge);
        v_vlbs[vertices.first][vertices.second] = true;
    }

    this->vectorized = true;
}

unsigned int Network::n_outgoing(vertex_key v) {
    if (this->outgoing.find(v) == this->outgoing.end()) {
        return 0;
    }
    return this->outgoing[v].size();
}

bool Network::exists_edge(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    return this->costs.find(edge) != this->costs.end();
}

bool Network::exists_path(vertex_key v_from, vertex_key v_to) const {
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
        if (this->outgoing.find(visiting) != this->outgoing.end()) {
            for (auto v : this->outgoing.at(visiting)) {
                if (visited.find(v) == visited.end()) {
                    to_visit.push(v);
                }
            }
        }
    }
    return false;
}

bool Network::respects_upper_bounds(const Flow &flow, bool report) const {
    for (auto edge_value : flow.values) {
        if (edge_value.second > this->capacities.at(edge_value.first)) {
            if (report) {
                std::cout << "over capacity" << std::endl;
            }
            return false;
        }
    }
    return true;
}

bool Network::respects_lower_bounds(const Flow &flow, bool report) const {
    for (auto edge_value : flow.values) {
        if (this->vlbs.find(edge_value.first) != this->vlbs.end()) {
            int minimum_quantity = this->minimum_quantities.at(edge_value.first);
            if (edge_value.second != 0 && edge_value.second < minimum_quantity) {
                if (report) {
                    std::cout << "min q invalid" << std::endl;
                }
                return false;
            }
        }
    }
    return true;
}

bool Network::respects_bounds(const Flow &flow, bool report) const {
    return this->respects_lower_bounds(flow, report) && this->respects_upper_bounds(flow, report);
}

int Network::total_cost(const Flow &flow) const {
    int cost = 0;
    for (auto edge_value : flow.values) {
        int edge_cost = this->costs.at(edge_value.first);
        cost += edge_cost * edge_value.second;
    }
    return cost;
}

std::set<edge_key> Network::detect_wannabe_active_vlbs(const Flow &flow) const {
    std::set<edge_key> active_vlbs;
    for (auto edge_value : flow.values) {
        if (this->vlbs.find(edge_value.first) != this->vlbs.end()) {
            int minimum_quantity = this->minimum_quantities.at(edge_value.first);
            if (edge_value.second > 0) {
                active_vlbs.insert(edge_value.first);
            }
        }
    }
    return active_vlbs;
}
