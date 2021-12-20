#include "flow.hpp"
#include "network.hpp"

#define MUTATE_VALUE_PERCENT 0.1

// - do we want to restrict the GA search space to admissible solutions only?
// - then the fitness function is just -flow_cost(f)
// - this might turn out to be impractical because it would make genetic ops
//   (generate random solution, mutate, crossover) hard to write
// - alternative is to pick only some restrictions for the search space (those that are easy
//   to work with, e.g. f conserves flow, f matches network, f_ij respects some bounds), and
//   punish breaking others through a more complex fitness function

double fitness(Network &network, Flow &flow);

Flow mutate(Network &network, const Flow &flow);
Flow crossover(Flow &f1, Flow &f2);

Flow ga_solver(Network &network, int population_size, float mutation_rate);

Flow random_admissible_flow(Network &network);
