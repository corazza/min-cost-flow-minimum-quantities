#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ilcplex/ilocplex.h>
#include "json.hpp"
using json = nlohmann::json;

#include "generator.hpp"
#include "network.hpp"
#include "ga_solver.hpp"
#include "util.hpp"
#include "cplex.hpp"
int main() {
    std::srand(time(NULL));

    std::ifstream i_params("generator_parameters.json");
    json j_params;
    i_params >> j_params;
    i_params.close();
    GeneratorParameters p = j_params.get<GeneratorParameters>();

    SolverParameters sp;
    sp.num_perturbations = 2;
    sp.generation_size = 100;
    sp.num_steps = 10000;
    sp.flow_value = 10;
    sp.best_of = 3;
    // sp.elitism = sp.generation_size / 20;

    Network network = generate_instance(p);
    std::cout << "generated problem instance..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto best_solution = ga_solver(network, sp, sp.num_steps / 25);
    auto end = std::chrono::high_resolution_clock::now();

    // auto last_generation = ga_solver(network, sp, 1);
    std::cout << "lowest cost: " << best_solution.cost << std::endl;
    std::cout << "time taken to calculate: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "(s)" << std::endl;
    /*
    start = std::chrono::high_resolution_clock::now();
    double cplex_solution = rjesenje_cplex(&network, sp.flow_value);
    end = std::chrono::high_resolution_clock::now();

    if (cplex_solution == -1) std::cout << "cplex didn't find a solution" << std::endl;
    else
        std::cout << "cplex solution is: " << cplex_solution << std::endl;

    std::cout << "time taken to calculate: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "(s)" << std::endl;*/
}
