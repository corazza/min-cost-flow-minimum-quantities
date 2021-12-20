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
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    this->costs[edge] = cost;
    this->capacities[edge] = capacity;
    this->minimum_quantities[edge] = minimum_quantity;
    this->vlb[edge] = vlb;

    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

void Network::remove_edge(vertex_key v_from, vertex_key v_to) {
    assert(!"Network::remove_edge unimplemented!");
}

int Network::capacity(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->capacities.find(edge) == this->capacities.end()) {
        return 0;
    } else {
        return this->capacities[edge];
    }
}


unsigned int Network::n_outgoing(vertex_key v) {
    if (this->outgoing.find(v) == this->outgoing.end()) {
        return 0;
    }
    return this->outgoing[v].size();
}

void Network::compute_effective_capacities() {
    this->vertex_effective_capacity[this->sink] = this->flow_value;

    std::queue<vertex_key> to_visit; // assumes no cycles
    std::set<vertex_key> to_visit_check;

    std::unordered_map<vertex_key, int> visit_count;
    int max_visits = 0;

    for (auto &key : this->incoming[this->sink]) {
        to_visit.push(key);
    }

    // std::cout << "before while" << std::endl;

    // int i = 0;

    while (!to_visit.empty()) {
        auto visiting = to_visit.front();
        to_visit.pop();
        to_visit_check.erase(visiting);

        // if (i % 1000 == 0) {
        //     if (visit_count.find(visiting) == visit_count.end()) {
        //         visit_count[visiting] = 0;
        //     } else {
        //         visit_count[visiting] += 1;
        //         int current_visits = visit_count[visiting];
        //         if (current_visits > max_visits) {
        //             max_visits = current_visits;
        //             std::cout << "visit record: " << visiting << " " << current_visits << " times, to_visit: " << to_visit.size() << std::endl;
        //         }
        //     }
        // }
        // ++i;

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

int Network::effective_capacity(vertex_key v) {
    if (this->vertex_effective_capacity.find(v) == this->vertex_effective_capacity.end()) {
        return 0;
    }
    return this->vertex_effective_capacity[v];
}

int Network::effective_capacity(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    if (this->edge_effective_capacity.find(edge) == this->edge_effective_capacity.end()) {
        return 0;
    }
    return this->edge_effective_capacity[edge];
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
