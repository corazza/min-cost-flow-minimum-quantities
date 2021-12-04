/* Our problem instance generator
 */

#ifndef GENERATOR_H
#define GENERATOR_H

#include "mcnfmq.hpp"

// TODO look at those three parameters other generators use, make non-uniform generator based on
// them

struct ParametersUniform {
    unsigned int n_nodes;
    unsigned int flow_value;
    unsigned int cost_max;      // edge cost is in [0, cost_max]
    unsigned int capacity_max;  // variable lower bound (minimum qunatity) is in [0, capacity_max]
                                // upper bound (capacity) is in [1, capacity_max]
};

// - chooses uniformly from the set of all digraphs over p.n_nodes vertices
// - assigns each edge a random cost, minimum quantity, and capacity (uniformly as defined by p)
// - therefore the returned instance might not have a solution (e.g. source and sink might be
//   disconnected)
Mcnfmq generate_uniform_instance(ParametersUniform p);

#endif
