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

bool Network::respects_bounds(Flow &flow) const {
    return true; // TODO
}

// // Efikasni algoritmi za rješavanje robusnih varijanti problema toka u mreži, Marko Špoljarec (2018., str. 30)
// Network layered_residual_network(Network &original) {
//     int i = 0;
//     std::vector<std::set<vertex_key> > layers;
//     std::set<vertex_key> first_layer;
//     first_layer.insert(original.source);
//     layers.push_back(first_layer);
// }
