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

    Parameters p;
    p.n_nodes = 50;
    p.max_span_q = p.n_nodes / 2;
    p.inclusion_p = 1;
    p.vlb_p = 0.1;
    p.flow_value = 50;
    p.cost_max = 20;
    p.alpha_1 = 4;
    p.alpha_2 = 5;
    p.alpha_3 = 10;
    p.alpha_4 = 10;

    FlowNetwork network = generate_instance(p);
    std::cout << "generated random network" << std::endl;
    std::cout << "source: " << network.network.source << std::endl;
    std::cout << "sink: " << network.network.sink << std::endl;

    auto random_flow1 = random_admissible_flow(network);
    std::cout << "first random flow:" << std::endl;
    random_flow1.print();
    std::cout << std::endl;

    auto random_flow2 = random_admissible_flow(network);
    std::cout << "second random flow:" << std::endl;
    random_flow2.print();
    std::cout << std::endl;

    auto mutated_flow = mutate(network, random_flow1);

    std::cout << "decomposing..." << std::endl;

    auto decomposed1 = decompose2(random_flow1, network);
    auto decomposed2 = decompose2(random_flow2, network);

    std::cout << "recomposing..." << std::endl;

    auto composed = compose(decomposed1, decomposed2, network);

    std::cout << "re-composed flow: " << std::endl;
    composed.print();

    return 0;
}
