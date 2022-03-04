#include "network.hpp"
#include "generator.hpp"
#include "cplex.hpp"
#include <chrono>
#include "ga_solver.hpp"
#include <stdlib.h>
#include <time.h>

#include "results.hpp"

int main() {
    srand(time(NULL));
    /*
    nodes = 4;
    Network graf(nodes);
    graf.add_edge(0, 1, 10, 4, 2, true);
    graf.add_edge(0, 2, 3, 5, 1, true);
    graf.add_edge(1, 3, 10, 3, 2, true);
    graf.add_edge(1, 2, 1, 7, 1, true);
    graf.add_edge(2, 3, 3, 4, 1, true);
    // */
    // int flow_value = 10;

    // GeneratorParameters p;
    // p.n_nodes = 50;
    // p.max_span_q = p.n_nodes / 2;
    // p.inclusion_p = 0.5;
    // p.vlb_p = 0.5;
    // p.cost_max = 20;
    // p.alpha_1 = 4;
    // p.alpha_2 = 5;
    // p.alpha_3 = 10;
    // p.alpha_4 = 10;
    std::ifstream i_params("generator_parameters.json");
    json j_params;
    i_params >> j_params;
    i_params.close();
    GeneratorParameters p = j_params.get<GeneratorParameters>();
    std::ifstream i_params2("solver_parameters.json");
    json j_params2;
    i_params2 >> j_params2;
    i_params2.close();
    SolverParameters sp = j_params2.get<SolverParameters>();
    sp.flow_value = p.flow_value;

    Network graf = generate_instance(p);
    std::cout << "n_edges: " << graf.n_edges() << std::endl;

    json j_network = graf; // Network network = ...;
    std::ofstream o_network("output_network.json");
    o_network << std::setw(4) << j_network << std::endl;
    o_network.close();

    auto pocetak = std::chrono::high_resolution_clock::now();

    Flow flow(0, graf.sink, graf.n_nodes, graf.max_span_q);

    double vrijeme_cplex = rjesenje_cplex(&graf, p.flow_value, &flow);
    if (vrijeme_cplex < 0) {
        std::cout << "CPLEX nije uspio pronaci rjesenje: probajte ponovno ili s drugim parametrima!" << std::endl;
        std::ofstream o_steps("results.json");
        o_steps << "[]" << std::endl;
        o_steps.close();
        return 0;
    }

    std::set<edge_key> variable_bounds = graf.detect_wannabe_active_vlbs(flow);

    auto kraj = std::chrono::high_resolution_clock::now();
    auto vrijeme = std::chrono::duration_cast<std::chrono::seconds>(kraj - pocetak);

    std::cout << "Rjesenje dobiveno cplexom: " << vrijeme_cplex << ", dobiveno u vremenu: " << vrijeme.count() << " s" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    auto best_solution_steps = ga_solver(graf, sp, sp.num_steps / 100);
    Solution best_solution = best_solution_steps.first;

    Solution solution(flow, variable_bounds);
    json j_solution = solution;
    std::ofstream o_flow("output_solution.json");
    o_flow << std::setw(4) << j_solution << std::endl;
    o_flow.close();

    auto steps = best_solution_steps.second;
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "GA lowest cost: " << best_solution.cost << std::endl;
    std::cout << "CPLEX solution: " << vrijeme_cplex << std::endl;
    std::cout << "time taken to calculate: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << "(s)" << std::endl;

    Results results(graf, best_solution, steps, vrijeme_cplex, sp, p);

    json j_results = results;
    std::ofstream o_steps("results.json");
    o_steps << std::setw(4) << j_results << std::endl;
    o_steps.close();
}
