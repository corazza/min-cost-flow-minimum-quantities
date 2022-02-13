#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include "generator.hpp"
#include "network.hpp"
#include "ga_solver.hpp"

int main() {
    std::srand(time(NULL));

    std::ifstream i_generator_params("generator_parameters.json");
    json j_generator_params;
    i_generator_params >> j_generator_params;
    i_generator_params.close();
    GeneratorParameters p = j_generator_params.get<GeneratorParameters>();

    std::ifstream i_solver_params("solver_parameters.json");
    json j_solver_params;
    i_solver_params >> j_solver_params;
    i_solver_params.close();
    SolverParameters sp = j_solver_params.get<SolverParameters>();

    Network network = generate_instance(p);
    std::cout << "generated random network" << std::endl;

    json j_network = network;
    std::ofstream o_network("output_network.json");
    o_network << std::setw(4) << j_network << std::endl;
    o_network.close();
    std::cout << "serialized network to output_network.json" << std::endl;

    auto solution = random_solution(network, sp.flow_value);
    std::cout << "generated random solution (flow, active vlbs)" << std::endl;

    json j_solution = solution;
    std::ofstream o_solution("output_solution.json");
    o_solution << std::setw(4) << j_solution << std::endl;
    o_solution.close();
    std::cout << "serialized solution to output_solution.json" << std::endl;

    // std::cout << "number of edges = " << network.n_edges() << std::endl;
    // float expected_n_edges = p.inclusion_p * p.max_span_q * (p.n_nodes - ((float)p.max_span_q+1)/2);
    // std::cout << "expected: " << expected_n_edges << std::endl;

    return 0;
}
