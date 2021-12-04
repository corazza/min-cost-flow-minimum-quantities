/* Our problem network generator
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "network.hpp"

// TODO look at those three parameters other generators use, make non-uniform generator based on
// them

struct Parameters {
    unsigned int n_nodes;
    unsigned int flow_value;
    unsigned int cost_max;      // edge cost is in [0, cost_max]
    unsigned int capacity_max;  // variable lower bound (minimum qunatity) is in [0, capacity_max]
                                // upper bound (capacity) is in [1, capacity_max]

    Parameters(unsigned int n_nodes, unsigned int flow_value, unsigned int cost_max,
               unsigned int capacity_max)
        : n_nodes(n_nodes),
          flow_value(flow_value),
          cost_max(cost_max),
          capacity_max(capacity_max) {}
};

std::pair<vertex_key, vertex_key> add_random_edge(Network* network, Parameters p);

// Builds Network of p.n_nodes nodes
// - edges are created at random, but a source->sink path is ensured, etc.
// - assigns each edge a random cost, capacity, and minimum quantity (as defined by p)
Network generate_instance(Parameters p);

#endif
