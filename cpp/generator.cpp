#include "generator.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <stack>

bool can_create_edge(Network* network, vertex_key v_from, vertex_key v_to) {
    bool result = v_from != v_to;                                              // are_different
    result = (v_from != network->sink) && result;                              // not_from_sink
    result = (v_to != network->source) && result;                              // not_to_source
    result = !(v_from == network->source && v_to == network->sink) && result;  // not_source_to_sink
    return result;
}

std::pair<vertex_key, vertex_key> add_random_edge(Network* network, Parameters p) {
    int v_from;
    int v_to;
    while (true) {
        v_from = rand() % p.n_nodes;
        v_to = rand() % p.n_nodes;
        if (can_create_edge(network, v_from, v_to) && !network->exists_edge(v_from, v_to)) {
            break;
        }
    }
    int cost = rand() % (p.cost_max + 1);
    int capacity = (rand() % p.capacity_max) + 1;
    int minimum_quantity = rand() % (p.capacity_max + 1);
    network->add_edge(v_from, v_to, cost, capacity, minimum_quantity);
    return std::make_pair(v_from, v_to);
}

Network generate_instance(Parameters p) {
    Network network(p.n_nodes, p.flow_value);
    std::set<vertex_key> reachable_from_source;
    reachable_from_source.insert(network.source);
    while (reachable_from_source.find(network.sink) == reachable_from_source.end()) {
        auto from_to = add_random_edge(&network, p);
        if (reachable_from_source.find(from_to.first) != reachable_from_source.end()) {
            std::set<vertex_key> visited;
            std::stack<vertex_key> to_visit;
            to_visit.push(from_to.second);
            while (!to_visit.empty()) {
                vertex_key visiting = to_visit.top();
                to_visit.pop();
                visited.insert(visiting);
                for (auto v : network.outgoing[visiting]) {
                    if (visited.find(v) == visited.end()) {
                        to_visit.push(v);
                    }
                }
            }
            for (auto v : visited) {
                reachable_from_source.insert(v);
            }
        }
    }
    return network;
}
