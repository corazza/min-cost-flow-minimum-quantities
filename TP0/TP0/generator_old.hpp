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

std::pair<vertex_key, vertex_key> add_random_edge(Network& network, Parameters p);

// TODO this generator is terrible, but makes valid problem instances
// Builds Network of p.n_nodes nodes
// - edges are created at random, but a source->sink path is ensured, etc.
// - assigns each edge a random cost, capacity, and minimum quantity (as defined by p)
Network generate_instance(Parameters p);

// data structure used by generate_instance2
struct Blueprint {
    unsigned int node_counter;
    vertex_key source;
    vertex_key sink;

    std::unordered_map<vertex_key, std::set<vertex_key> > outgoing;
    std::unordered_map<vertex_key, std::set<vertex_key> > incoming;

    vertex_key add_node();
    void add_edge(vertex_key v_from, vertex_key v_to);
    void remove_edge(vertex_key v_from, vertex_key v_to);
    bool exists_edge(vertex_key v_from, vertex_key v_to);  // considers direction
    vertex_key random_regular_node();

    Blueprint() : node_counter(0) {
        this->source = this->add_node();
        this->sink = this->add_node();
        vertex_key v = this->add_node();
        this->add_edge(this->source, v);
        this->add_edge(v, this->sink);
    }
};

#define BP_ACTION_PAR 1
#define BP_ACTION_SEQ 2

Network blueprint_to_network(Blueprint blueprint, Parameters p);
void execute_action(Blueprint& blueprint, vertex_key node, int action);
// better generator
Network generate_instance2(Parameters p);

// generator from paper (1)
Network generate_instance_paper_one(Parameters p);

#endif
