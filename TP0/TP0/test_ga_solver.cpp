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
    p.vlb_p = 0.05;
    p.flow_value = 50;
    p.cost_max = 20;
    p.alpha_1 = 4;
    p.alpha_2 = 5;
    p.alpha_3 = 10;
    p.alpha_4 = 10;

    Network network = generate_instance(p);

    auto vlbs_flow1 = random_admissible_flow(network, p.flow_value, p.flow_value / 2);
    auto active_vlbs1 = vlbs_flow1.first;
    auto random_flow1 = vlbs_flow1.second;

    auto vlbs_flow2 = random_admissible_flow(network, p.flow_value, p.flow_value / 2);
    auto active_vlbs2 = vlbs_flow2.first;
    auto random_flow2 = vlbs_flow2.second;

    auto mutated_flow = mutate(network, random_flow1, active_vlbs1, 5);
    auto crossover_flow = crossover(network, random_flow1, random_flow2);
}
