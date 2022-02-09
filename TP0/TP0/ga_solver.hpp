#ifndef GA_SOLVER_H
#define GA_SOLVER_H

#include "flow.hpp"
#include "network.hpp"
#include <list>
#define MUTATE_VALUE_PERCENT 0.1

std::vector<vertex_key> find_augmenting_path(const Network &network, Flow &flow, vertex_key v_from, vertex_key v_to, int value);
std::set<edge_key> random_active_vlbs(const Network &network, int up_to_flow_value);
Flow random_admissible_flow_throws(const Network &network, int flow_value, std::set<edge_key> active_vlbs);
std::pair<std::set<edge_key>, Flow> random_admissible_flow(const Network &network, int flow_value, int up_to_value);
Flow fix_vlbs(const Network &network, const Flow &original, std::set<edge_key> active_vlbs);
Flow fix_flow_value(const Network &network, const Flow &original, std::set<edge_key> active_vlbs, int flow_value);

double fitness(Network &network, Flow &flow);

Flow mutate(const Network &network, const Flow &original, std::set<edge_key> active_vlbs, int num_perturbations);
Flow crossover(const Network &network, Flow &f1, Flow &f2);

std::vector<Flow> decompose(const Flow &f, const Network& network);
Flow compose_throws(std::vector<Flow> &decom1, std::vector<Flow> &decom2, std::set<edge_key> active_vlbs, const Network& network);
std::pair<std::set<edge_key>, Flow> compose(std::vector<Flow> &decom1, std::vector<Flow> &decom2, const Network& network);

Flow ga_solver(const Network &network, int population_size, float mutation_rate);

#endif
