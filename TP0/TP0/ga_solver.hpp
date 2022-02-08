#include "flow.hpp"
#include "network.hpp"
#include <list>
#define MUTATE_VALUE_PERCENT 0.1

double fitness(Network &network, Flow &flow);

Flow mutate(Network &network, Flow &flow);
Flow crossover(Flow &f1, Flow &f2);

std::vector<Flow> decompose(const Flow &f, const Network& network);
std::vector<Flow> decompose2(const Flow &f, const Network& network);
Flow compose(std::vector<Flow>& f1, std::vector<Flow>& f2, const Network& network);

Flow ga_solver(Network &network, int population_size, float mutation_rate);

std::set<edge_key> random_active_vlbs(Network &network, int up_to_flow_value);
Flow random_admissible_flow(Network &network, int flow_value, std::set<edge_key> active_vlbs);

std::vector<vertex_key> find_augmenting_path(Network &network, Flow &flow, vertex_key v_from, vertex_key v_to, int value);

// -> 43 (21)

// 25