#include "network.hpp"
#include "generator.hpp"
#include "cplex.hpp"
#include <chrono>
#include "ga_solver.hpp"


int main() {
    int nodes;
    /*
    nodes = 4;
    Network graf(nodes);
    graf.add_edge(0, 1, 10, 4, 2, true);
    graf.add_edge(0, 2, 3, 5, 1, true);
    graf.add_edge(1, 3, 10, 3, 2, true);
    graf.add_edge(1, 2, 1, 7, 1, true);
    graf.add_edge(2, 3, 3, 4, 1, true);
    */
    int flow_value = 10;
    
    GeneratorParameters p;
    p.n_nodes = 5;
    p.max_span_q = p.n_nodes / 2;
    p.inclusion_p = 1;
    p.vlb_p = 0.5;
    p.cost_max = 20;
    p.alpha_1 = 4;
    p.alpha_2 = 5;
    p.alpha_3 = 10;
    p.alpha_4 = 10;
    
    Network graf = generate_instance(p);
    

    json j_network = graf; // Network network = ...;
    std::ofstream o_network("output_network.json");
    o_network << std::setw(4) << j_network << std::endl;
    o_network.close();

	auto pocetak = std::chrono::high_resolution_clock::now();

    Flow flow(0, graf.sink);
    std::set<edge_key> variable_bounds;
	double vrijeme_cplex = rjesenje_cplex(&graf, flow_value, &flow, &variable_bounds);

    Solution solution(flow, variable_bounds);
    json j_solution = solution; 
    std::ofstream o_flow("output_solution.json");
    o_flow << std::setw(4) << j_solution << std::endl;
    o_flow.close();

	auto kraj = std::chrono::high_resolution_clock::now();
	auto vrijeme = std::chrono::duration_cast<std::chrono::seconds>(kraj - pocetak);

	std::cout << "Rjesenje dobiveno cplexom: " << vrijeme_cplex << ", dobiveno u vremenu: " << vrijeme.count() << " s" << std::endl;

	return 0;
}

