#ifndef GENERATOR_H
#define GENERATOR_H

#include "network.hpp"

struct GeneratorParameters {
    unsigned int n_nodes;
    unsigned int max_span_q; // 1 < q <= n_nodes. for node (i, j): j - i <= q
    float inclusion_p;
    float vlb_p;
    unsigned int cost_max;      // edge cost is in [0, cost_max]
    unsigned int alpha_1; // lower bound = 0 if not vlb
    unsigned int alpha_2; // lower bound = alpha_1 + alpha_2 * Unif(0, 1) if vlb
    unsigned int alpha_3; // capacity = alpha_3 + alpha_4 * Unif(0, 1)
    unsigned int alpha_4; // alpha_3 > alpha_1 + alph_2
    unsigned int flow_value; // convenience

};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GeneratorParameters, n_nodes, max_span_q, inclusion_p, vlb_p, cost_max, alpha_1, alpha_2, alpha_3, alpha_4, flow_value);

Network generate_instance(GeneratorParameters p);

#endif
