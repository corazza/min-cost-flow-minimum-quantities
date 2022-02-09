#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include "generator.hpp"
#include "network.hpp"
#include "ga_solver.hpp"
#include "util.hpp"

int main() {
    std::srand(time(NULL));

    GeneratorParameters p;
    p.n_nodes = 50;
    p.max_span_q = p.n_nodes / 2;
    p.inclusion_p = 0.5;
    p.vlb_p = 0.05;
    p.cost_max = 20;
    p.alpha_1 = 4;
    p.alpha_2 = 5;
    p.alpha_3 = 10;
    p.alpha_4 = 10;

    SolverParameters sp;
    sp.num_perturbations = 2;
    sp.generation_size = 100;
    sp.num_steps = 10000;
    sp.flow_value = 10;
    sp.best_of = 3;
    // sp.elitism = sp.generation_size / 20;

    Network network = generate_instance(p);
    std::cout << "generated problem instance..." << std::endl;
    auto best_solution = ga_solver(network, sp, sp.num_steps / 25);
    // auto last_generation = ga_solver(network, sp, 1);
    std::cout << "lowest cost: " << best_solution.cost << std::endl;
}
