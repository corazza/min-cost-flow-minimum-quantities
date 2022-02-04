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
    p.n_nodes = 200;
    p.max_span_q = p.n_nodes / 2;
    p.inclusion_p = 0.5;
    p.vlb_p = 0.2;
    p.flow_value = 20;
    p.cost_max = 20;
    p.alpha_1 = 10;
    p.alpha_2 = 10;
    p.alpha_3 = 21;
    p.alpha_4 = 10;

    Network network = generate_instance(p);
    std::cout << "generated random network" << std::endl;
    std::cout << "source: " << network.source << std::endl;
    std::cout << "sink: " << network.sink << std::endl;

    auto random_flow = random_admissible_flow(network);
    std::cout << "random flow: " << std::endl;
    // random_flow.print();
    std::cout << "value=" << random_flow.recompute_value(network) << std::endl;
    // std::cout << std::endl;

    // auto mutated_flow = mutate(network, random_flow);
    // std::cout << "mutated flow: " << std::endl;
    // mutated_flow.print();
    // std::cout << "value=" << mutated_flow.recompute_value(network) << std::endl;
    // std::cout << std::endl;

    // TODO crossover

    auto decomposed = decompose(random_flow, network);
    std::cout << "decomposed into " << decomposed.size() << " flows" << std::endl;


    for (int i = 0; i < decomposed.size(); ++i) {}

    return 0;
}
