#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include "generator.hpp"
#include "network.hpp"
#include "ga_solver.hpp"

void check_difference(Flow one, Flow two) {
    int changed = 0;
    int total = 0;
    int difference = 0;
    int value_one = 0;
    int value_two = 0;

    std::set<edge_key> edges;
    for (auto edge_value : one.values) {
        edges.insert(edge_value.first);
    }
    for (auto edge_value : two.values) {
        edges.insert(edge_value.first);
    }

    for (auto edge : edges) {
        int value_in_one = one.edge_value(edge);
        int value_in_two = two.edge_value(edge);
        difference += abs(value_in_two - value_in_one);
        value_one += value_in_one;
        value_two += value_in_two;
        if (value_in_one != value_in_two) {
            auto vertices = get_vertex_keys(edge);
            vertex_key v_from = vertices.first;
            vertex_key v_to = vertices.second;
            // std::cout << "changed " << v_from << " -> " << v_to << ", one=" << value_in_one << ", two=" << value_in_two << std::endl;
            ++changed;
        }
        if (value_in_one != 0 || value_in_two != 0) {
            ++total;
        }
    }

    std::cout << "changed=" << changed << ", total=" << total << std::endl;
    std::cout << "value_one=" << value_one << ", value_two=" << value_two << ", difference=" << difference << std::endl;
}

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

    for (int i = 0; i < 100; ++i) {


    Network network = generate_instance(p);
    std::cout << "generated random network" << std::endl;
    std::cout << "source: " << network.source << std::endl;
    std::cout << "sink: " << network.sink << std::endl;

    auto vlbs_flow1 = random_admissible_flow(network, p.flow_value, p.flow_value / 2);
    auto active_vlbs1 = vlbs_flow1.first;
    std::cout << "active 1: " << active_vlbs1.size() << std::endl;
    auto random_flow1 = vlbs_flow1.second;

    auto vlbs_flow2 = random_admissible_flow(network, p.flow_value, p.flow_value / 2);
    auto active_vlbs2 = vlbs_flow2.first;
    std::cout << "active 2: " << active_vlbs2.size() << std::endl;
    auto random_flow2 = vlbs_flow2.second;

    auto mutated_flow = mutate(network, random_flow1, active_vlbs1, 5);

    std::cout << "decomposing..." << std::endl;

    std::vector<Flow> decomposed1 = decompose(random_flow1, network);
    std::vector<Flow> decomposed2 = decompose(random_flow2, network);

    std::cout << "recomposing..." << std::endl;

    std::pair<std::set<edge_key>, Flow> recomposed = compose(decomposed1, decomposed2, network);
    auto recomposed_vlbs = recomposed.first;
    auto recomposed_flow = recomposed.second;

    std::cout << "random_flow1 vs. recomposed" << std::endl;
    check_difference(random_flow1, recomposed_flow);
    std::cout << std::endl;
    std::cout << "random_flow2 vs. recomposed" << std::endl;
    check_difference(random_flow2, recomposed_flow);
    std::cout << std::endl;
    std::cout << "random_flow1 vs. random_flow2" << std::endl;
    check_difference(random_flow1, random_flow2);
    std::cout << std::endl;

    // std::cout << "re-composed flow: " << std::endl;
    // composed.print();
    }
}
