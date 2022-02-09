#include "network.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <stack>
#include <queue>

void Network::add_edge(vertex_key v_from, vertex_key v_to, int cost, int capacity,
                       int minimum_quantity, bool vlb) {
    assert(v_from != v_to);
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
}

int Network::capacity(vertex_key v_from, vertex_key v_to) const {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->capacities.find(edge) == this->capacities.end()) {
        return 0;
    } else {
        return this->capacities.at(edge);
    }
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

bool Network::respects_upper_bounds(const Flow &flow) const {
    for (auto edge_value : flow.values) {
        if (edge_value.second > this->capacities.at(edge_value.first)) {
            // std::cout << "over capacity" << std::endl;
            return false;
        }
    }
    return true;
}

bool Network::respects_lower_bounds(const Flow &flow) const {
    for (auto edge_value : flow.values) {
        if (this->vlbs.find(edge_value.first) != this->vlbs.end()) {
            int minimum_quantity = this->minimum_quantities.at(edge_value.first);
            if (edge_value.second != 0 && edge_value.second < minimum_quantity) {
                // std::cout << "min q invalid" << std::endl;
                return false;
            }
        }
    }
    return true;
}

bool Network::respects_bounds(const Flow &flow) const {
    return this->respects_lower_bounds(flow) && this->respects_upper_bounds(flow);
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
