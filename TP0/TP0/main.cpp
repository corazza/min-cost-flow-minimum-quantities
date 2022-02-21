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
    
    GeneratorParameters p;
    p.n_nodes = nodes = 800;
    p.max_span_q = p.n_nodes / 2;
    p.inclusion_p = 0.5;
    p.vlb_p = 0.5;
    p.cost_max = 20;
    p.alpha_1 = 4;
    p.alpha_2 = 5;
    p.alpha_3 = 10;
    p.alpha_4 = 10;

    Network graf = generate_instance(p);
    
    

	auto pocetak = std::chrono::high_resolution_clock::now();

	double vrijeme_cplex = rjesenje_cplex(&graf, nodes);

	auto kraj = std::chrono::high_resolution_clock::now();
	auto vrijeme = std::chrono::duration_cast<std::chrono::seconds>(kraj - pocetak);

	std::cout << "Rjesenje dobiveno cplexom: " << vrijeme_cplex << ", dobiveno u vremenu: " << vrijeme.count() << " s" << std::endl;

	return 0;
}

