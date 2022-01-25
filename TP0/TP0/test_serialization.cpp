#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include "generator.hpp"
#include "network.hpp"
#include "ga_solver.hpp"

/*
int main() {
    std::srand(time(NULL));

    Parameters p;
    p.n_nodes = 200;
    p.max_span_q = p.n_nodes / 2;
    p.inclusion_p = 0.5;
    p.vlb_p = 0.2;
    p.flow_value = 500;
    p.cost_max = 20;
    p.alpha_1 = 10;
    p.alpha_2 = 20;
    p.alpha_3 = 31;
    p.alpha_4 = 41;

    Network network = generate_instance(p);
    std::cout << "generated random network" << std::endl;

    json j = network;
    std::ofstream o("output.json");
    o << std::setw(4) << j << std::endl;
    o.close();
    std::cout << "serialized network to output.json" << std::endl;

    std::ifstream i("output.json");
    json j2;
    i >> j2;
    i.close();

    Network instance2 = j2.get<Network>();
    std::cout << "deserialized network from output.json" << std::endl;

    std::cout << "number of edges = " << network.n_edges() << std::endl;
    float expected_n_edges = p.inclusion_p * p.max_span_q * (p.n_nodes - ((float)p.max_span_q+1)/2);
    std::cout << "expected: " << expected_n_edges << std::endl;

    return 0;
}

*/
