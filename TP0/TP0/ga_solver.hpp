#ifndef GA_SOLVER_H
#define GA_SOLVER_H

#include "flow.hpp"
#include "network.hpp"
#include <list>

struct SolverParameters {
    int num_perturbations;
    int generation_size;
    int num_steps;
    int flow_value;
    int best_of;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SolverParameters, num_perturbations, generation_size, num_steps, flow_value, best_of);

struct Solution {
    Flow flow;
    std::set<edge_key> active_vlbs;
    int cost;

    Solution make_copy() {
        Flow flow = this->flow.make_copy();
        auto active_vlbs = this->active_vlbs;
        Solution copy(flow, active_vlbs);
        copy.cost = this->cost;
        return copy;
    }

    Solution(Flow flow, std::set<edge_key> active_vlbs) : flow(flow), active_vlbs(active_vlbs), cost(-1) {}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Solution, flow, active_vlbs, cost);

// double fitness(Network &network, Flow &flow);
// std::pair<std::set<edge_key>, Flow> random_admissible_flow(const Network &network, int flow_value, int up_to_value);
// std::pair<std::set<edge_key>, Flow> mutate(const Network &network, const Flow &original, std::set<edge_key> active_vlbs, int num_perturbations);
// std::pair<std::set<edge_key>, Flow> crossover(const Network &network, Flow &f1, Flow &f2);
Solution random_solution(const Network &network, int flow_value);
Solution ga_solver(const Network &network, const SolverParameters &sp, int report_every);

void check_difference(const Flow &one, const Flow &two);

#endif
