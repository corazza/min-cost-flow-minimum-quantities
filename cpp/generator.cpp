#include "generator.hpp"

#include <iostream>

Mcnfmq generate_uniform_instance(ParametersUniform p) {
    Mcnfmq instance(p.n_nodes, 0, 1, p.flow_value);
    int max_edges = p.n_nodes * (p.n_nodes - 1);
    int n_edges = (std::rand() % max_edges) + 1;
    for (int i = 0; i < n_edges; ++i) {
        int v_from = rand() % p.n_nodes;
        int v_to = rand() % p.n_nodes;
        if (v_from == v_to) {
            --i;
            continue;
        }
        edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
        instance.costs[edge] = rand() % (p.cost_max + 1);
        instance.minimum_quantities[edge] = rand() % (p.capacity_max + 1);
        instance.capacities[edge] = (rand() % p.capacity_max) + 1;
    }
    return instance;
}
