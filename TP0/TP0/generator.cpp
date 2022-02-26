#include "generator.hpp"
#include "util.hpp"

#include <cassert>
#include <iostream>
#include <set>
#include <stack>

Network generate_instance(GeneratorParameters p) {
    Network network(p.n_nodes);
    network.max_span_q = p.max_span_q;

    assert(network.source == 0 && network.sink == network.n_nodes - 1);
    assert(p.alpha_3 > p.alpha_1 + p.alpha_2);

    for (int i = 0; i < p.n_nodes; ++i) {
        int k = min(i+p.max_span_q, p.n_nodes);
        for (int j = i+1; j < k; ++j) {
            float x = (float)rand() / (float)RAND_MAX;
            if (x > p.inclusion_p) {
                continue;
            }
            int cost = rand() % (p.cost_max + 1);
            bool vlb = true;
            x = (float)rand() / (float)RAND_MAX;
            if (x > p.vlb_p) {
                vlb = false;
            }
            int minimum_quantity = 0;
            if (vlb) {
                minimum_quantity = p.alpha_1 + (rand() % (p.alpha_2+1));
            }
            int capacity = p.alpha_3 + (rand() % (p.alpha_4+1));
            network.add_edge(i, j, cost, capacity, minimum_quantity, vlb);
        }
    }

    network.vectorize();

    return network;
}
