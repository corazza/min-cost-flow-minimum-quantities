#include "network.hpp"
#include "generator.hpp"
#include "cplex.hpp"
#include <chrono>
int main() {

	Network graf(4, 4, 2);
	graf.add_edge(0, 2, 10, 4, 2, true);
	graf.add_edge(0, 3, 3, 5, 1, true);
	graf.add_edge(2, 1, 10, 3, 2, true);
	graf.add_edge(2, 3, 1, 7, 1, true);
	graf.add_edge(3, 1, 3, 4, 1, true);

	auto pocetak = std::chrono::high_resolution_clock::now();

	double vrijeme_cplex = rjesenje_cplex(&graf);

	auto kraj = std::chrono::high_resolution_clock::now();
	auto vrijeme = std::chrono::duration_cast<std::chrono::milliseconds>(kraj - pocetak);

	std::cout << "Rjesenje dobiveno cplexom: " << vrijeme_cplex << ", dobiveno u vremenu: " << vrijeme.count() << " ms" << std::endl;

	return 0;
}