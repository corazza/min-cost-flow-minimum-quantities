#include "network.hpp"
#include "util.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <stack>
#include <queue>

void FlowNetwork::add_edge(vertex_key v_from, vertex_key v_to, int cost, int capacity,
                       int minimum_quantity, bool vlb) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    this->costs[edge] = cost;
    this->capacities[edge] = capacity;
    this->minimum_quantities[edge] = minimum_quantity;
    this->vlb[edge] = vlb;

    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

void FlowNetwork::remove_edge(vertex_key v_from, vertex_key v_to) {
    assert(!"FlowNetwork::remove_edge unimplemented!");
}

bool FlowNetwork::respects_bounds(Flow &flow) const {
    return true; // TODO
}

int FlowNetwork::capacity(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->capacities.find(edge) == this->capacities.end()) {
        return 0;
    } else {
        return this->capacities[edge];
    }
}


unsigned int FlowNetwork::n_outgoing(vertex_key v) {
    if (this->outgoing.find(v) == this->outgoing.end()) {
        return 0;
    }
    return this->outgoing[v].size();
}

void FlowNetwork::compute_effective_capacities() {
    for (int i = this->source; i <= this->sink; ++i) {
        this->vertex_effective_capacity[i] = 0;
    }

    this->vertex_effective_capacity[this->sink] = this->flow_value;

    std::queue<vertex_key> to_visit; // assumes no cycles
    std::set<vertex_key> to_visit_check;

    for (auto &key : this->incoming[this->sink]) {
        to_visit.push(key);
    }

    while (!to_visit.empty()) {
        auto visiting = to_visit.front();
        to_visit.pop();
        to_visit_check.erase(visiting);

        for (auto &key : this->incoming[visiting]) {
            if (to_visit_check.find(key) == to_visit_check.end()) {
                to_visit.push(key);
                to_visit_check.insert(key);
            }
        }

        int outgoing_capacity = 0;
        for (auto &neighbor : this->outgoing[visiting]) {
            edge_key edge = get_edge_key(visiting, neighbor);
            int capacity = this->capacities[edge];
            int neighbor_effective_capacity = this->effective_capacity(neighbor);
            outgoing_capacity += min(capacity, neighbor_effective_capacity);
        }
        this->vertex_effective_capacity[visiting] = outgoing_capacity;
    }

    // std::cout << "after while" << std::endl;

    assert(this->vertex_effective_capacity[this->source] >= this->flow_value);
    this->computed_effective_capacities = true;
}

int FlowNetwork::effective_capacity(vertex_key v) {
    if (this->vertex_effective_capacity.find(v) == this->vertex_effective_capacity.end()) {
        return 0;
    }
    return this->vertex_effective_capacity[v];
}

int FlowNetwork::effective_capacity(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    if (this->edge_effective_capacity.find(edge) == this->edge_effective_capacity.end()) {
        return 0;
    }
    return this->edge_effective_capacity[edge];
}

bool FlowNetwork::exists_edge(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    return this->costs.find(edge) != this->costs.end();
}

bool FlowNetwork::exists_path(vertex_key v_from, vertex_key v_to) {
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
