#ifndef RESULTS_H
#define RESULTS_H

#include <utility>

#include "ga_solver.hpp"

struct Results { 
    Solution best_solution;
    SolverParameters solver_parameters;
    GeneratorParameters generator_parameters;
    std::vector<std::pair<int, int> > steps;
    int cplex_cost;

    Results(Solution best_solution, std::vector<std::pair<int, int> > steps, int cplex_cost, SolverParameters solver_parameters, GeneratorParameters generator_parameters)
        : best_solution(best_solution), steps(steps), cplex_cost(cplex_cost), solver_parameters(solver_parameters), generator_parameters(generator_parameters) {}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Results, best_solution, steps, cplex_cost, solver_parameters, generator_parameters);

#endif
