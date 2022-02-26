#include "ga_solver.hpp"
#include "util.hpp"

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>
#include <algorithm>


std::vector<vertex_key> find_random_augmenting_path(const Network &network, Flow &flow, std::set<edge_key> &active_vlbs, vertex_key v_start, vertex_key v_end, int value, bool ignore_active_vlbs, bool ignore_inactive_vlbs) {
    assert(network.exists_path(v_start, v_end));
    std::set<vertex_key> blacklisted;
    std::unordered_map<vertex_key, vertex_key> parents;
    vertex_key visiting = v_start;

    while (visiting != v_end) {
        std::vector<vertex_key> suitable_neighbors;
        if (network.outgoing.find(visiting) != network.outgoing.end()) {
            std::set<vertex_key> neighbors = network.outgoing.at(visiting);
            for (auto neighbor : neighbors) {
                if (blacklisted.find(neighbor) != blacklisted.end()) {
                    continue;
                }
                auto edge = get_edge_key(visiting, neighbor);
                // bool is_vlb = network.vlbs.find(edge) != network.vlbs.end();
                bool is_vlb = network.v_vlbs[visiting][neighbor];
                bool is_active_vlb = active_vlbs.find(edge) != active_vlbs.end();
                assert(!is_active_vlb || is_vlb);
                // int minimum_quantity = network.minimum_quantities.at(edge);
                int minimum_quantity = network.v_minimum_quantities[visiting][neighbor];
                int current_value = flow.edge_value(visiting, neighbor);
                // int current_value = flow.v_values[visiting][neighbor];
                int new_value = current_value + value;

                bool within_bounds = new_value <= network.v_capacities[visiting][neighbor];
                within_bounds &= new_value >= 0;

                if (is_active_vlb) {
                    within_bounds &= (new_value >= minimum_quantity) || ignore_active_vlbs;
                } else if (is_vlb) {
                    within_bounds &= new_value == 0 || ignore_inactive_vlbs;
                }

                if (within_bounds) {
                    suitable_neighbors.push_back(neighbor);
                }
            }
        }

        int num_suitable_neighbors = suitable_neighbors.size();
        if (num_suitable_neighbors == 0) {
            if (visiting == v_start) {
                throw "visiting start with no suitable neighbors";
            }
            blacklisted.insert(visiting);
            visiting = parents.at(visiting);
        } else {
            int x = rand() % num_suitable_neighbors;
            vertex_key next = suitable_neighbors[x];
            parents[next] = visiting;
            visiting = next;
        }
    }

    std::vector<vertex_key> path;
    vertex_key current_vertex = v_end;
    while (current_vertex != v_start) {
        path.push_back(current_vertex);
        current_vertex = parents[current_vertex];
    }
    path.push_back(v_start);

    reverse(path.begin(), path.end());
    return path;
}

void apply_augmenting_path(const Network &network, Flow &flow, std::set<edge_key> active_vlbs, std::vector<vertex_key> &path, int value) {
    vertex_key current_vertex = path[0];
    for (int i = 1; i < path.size(); ++i) {
        vertex_key next_vertex = path[i];
        edge_key edge = get_edge_key(current_vertex, next_vertex);
        flow.add_to_edge(current_vertex, next_vertex, value);
        // flow.ensure_edge(current_vertex, next_vertex);
        // flow.v_values[current_vertex][next_vertex] += value;
        current_vertex = next_vertex;
    }
}

template<typename S>
auto select_random(const S &s, size_t n) {
  auto it = std::begin(s);
  // 'advance' the iterator n times
  std::advance(it, n);
  return it;
}

std::set<edge_key> random_active_vlbs(const Network &network, int up_to_flow_value) {
    std::set<edge_key> active_vlbs;
    std::set<edge_key> available_vlbs = network.vlbs;
    int num_active_vlbs = 0;
    if(available_vlbs.size() != 0) num_active_vlbs = rand() % available_vlbs.size();
    int added = 0;

    while (available_vlbs.size() > 0 && active_vlbs.size() < num_active_vlbs && added < up_to_flow_value) {
        int x = rand() % available_vlbs.size();
        auto edge = *select_random(available_vlbs, x);
        available_vlbs.erase(edge);
        auto vertices = get_vertex_keys(edge);
        vertex_key v_from = vertices.first;
        vertex_key v_to = vertices.second;

        assert(network.exists_path(v_from, v_to));
        if (!network.exists_path(network.source, v_from) || !network.exists_path(v_to, network.sink)) {
            continue;
        }

        // int minimum_quantity = network.minimum_quantities.at(edge);
        int minimum_quantity = network.v_minimum_quantities[v_from][v_to];
        added += minimum_quantity;
        if (added <= up_to_flow_value) {
            active_vlbs.insert(edge);
        }
    }

    return active_vlbs;
}

// should be called before fix_flow_value
Flow fix_active_vlbs(const Network &network, const Flow &original, std::set<edge_key> active_vlbs) {
    Flow flow = original.make_copy();
    // std::cout << "+ here 1" << std::endl;

    for (auto edge : active_vlbs) {
        // int minimum_quantity = network.minimum_quantities.at(edge);
        auto vertices = get_vertex_keys(edge);
        vertex_key v_from = vertices.first;
        vertex_key v_to = vertices.second;
        int minimum_quantity = network.v_minimum_quantities[v_from][v_to];

        assert(network.exists_path(network.source, v_from));
        assert(network.exists_path(v_to, network.sink));
        int current_value = flow.edge_value(v_from, v_to);
        // int current_value = flow.v_values[v_from][v_to];
        int debt = minimum_quantity - current_value;
        for (int i = 0; i < debt; ++i) {
            std::vector<vertex_key> source_to_v_from = find_random_augmenting_path(network, flow, active_vlbs, network.source, v_from, 1, true, false);
            apply_augmenting_path(network, flow, active_vlbs, source_to_v_from, 1);
            std::vector<vertex_key> v_to_to_sink = find_random_augmenting_path(network, flow, active_vlbs, v_to, network.sink, 1, true, false);
            apply_augmenting_path(network, flow, active_vlbs, v_to_to_sink, 1);
            flow.add_to_edge(v_from, v_to, 1);
            // flow.ensure_edge(v_from, v_to);
            // flow.v_values[v_from][v_to] += 1;
        }
    }
    // std::cout << "+ here 2" << std::endl;
    assert(flow.respects_flow_conservation());
    return flow;
}

// should be called before fix_flow_value
Flow fix_inactive_vlbs(const Network &network, const Flow &original, std::set<edge_key> active_vlbs) {
    Flow flow = original.make_copy();

    for (auto edge : network.vlbs) {
        if (active_vlbs.find(edge) != active_vlbs.end()) {
            continue;
        }
        auto vertices = get_vertex_keys(edge);
        vertex_key v_from = vertices.first;
        vertex_key v_to = vertices.second;
        int current_value = flow.edge_value(v_from, v_to);
        // int current_value = flow.v_values[v_from][v_to];
        int debt = current_value;
        for (int i = 0; i < debt; ++i) {
            std::vector<vertex_key> source_to_v_from = find_random_augmenting_path(network, flow, active_vlbs, network.source, v_from, -1, false, true);
            apply_augmenting_path(network, flow, active_vlbs, source_to_v_from, -1);
            std::vector<vertex_key> v_to_to_sink = find_random_augmenting_path(network, flow, active_vlbs, v_to, network.sink, -1, false, true);
            apply_augmenting_path(network, flow, active_vlbs, v_to_to_sink, -1);
            flow.add_to_edge(v_from, v_to, -1);
            // flow.ensure_edge(v_from, v_to);
            // flow.v_values[v_from][v_to] -= 1;
        }
    }
    assert(flow.respects_flow_conservation());
    return flow;
}

// assumes vlbs are fixed and bounds are respected
Flow fix_flow_value(const Network &network, const Flow &original, std::set<edge_key> active_vlbs, int flow_value) {
    assert(network.respects_bounds(original, false));
    Flow flow = original.make_copy();

    int original_value = original.flow_value();
    int debt = flow_value - original_value;
    if (debt == 0) {
        return original;
    }

    int increment = debt > 0 ? 1 : -1;
    int steps = abs(debt);

    for (int i = 0; i < steps; ++i) {
        std::vector<vertex_key> source_to_sink = find_random_augmenting_path(network, flow, active_vlbs, network.source, network.sink, increment, false, false);
        apply_augmenting_path(network, flow, active_vlbs, source_to_sink, increment);
    }

    assert(flow.respects_flow_conservation());
    assert(network.respects_bounds(flow, false));
    assert(flow.flow_value() == flow_value);

    return flow;
}

// main function that generates a random admissible flow, can throw an exception
Flow random_admissible_flow_throws(const Network &network, int flow_value, std::set<edge_key> active_vlbs) {
    Flow flow(network.source, network.sink, network.n_nodes, network.max_span_q);
    // std::cout << "here 1, " << active_vlbs.size() << std::endl;
    // flow.vectorize();
    // std::cout << "here 2" << std::endl;
    flow = fix_active_vlbs(network, flow, active_vlbs);
    // std::cout << "here 3" << std::endl;
    flow = fix_inactive_vlbs(network, flow, active_vlbs);
    // std::cout << "here 4" << std::endl;
    flow = fix_flow_value(network, flow, active_vlbs, flow_value);
    // std::cout << "here 5" << std::endl;
    assert(flow.respects_flow_conservation());
    assert(flow.flow_value() == flow_value);
    assert(network.respects_bounds(flow, false));
    return flow;
}

// TODO FIXME is the reason it's necessary to try/catch because the network can have dead-end active vlbs?
std::pair<std::set<edge_key>, Flow> random_admissible_flow(const Network &network, int flow_value, int up_to_value, std::set<std::set<edge_key > > &tried_active_vlbs) {
    int attempts = 0;
    while (true) {
        ++attempts;
        try {
            std::set<edge_key> active_vlbs = random_active_vlbs(network, up_to_value);
            // if (tried_active_vlbs.find(active_vlbs) != tried_active_vlbs.end()) {
            //     std::cout << "HIT FOUND" << std::endl;
            //     continue;
            // }
            // tried_active_vlbs.insert(active_vlbs);
            Flow flow = random_admissible_flow_throws(network, flow_value, active_vlbs);
            // if (attempts > 1) {
            //     std::cout << "took " << attempts << " attempts" << std::endl;
            // }
            return std::make_pair(active_vlbs, flow);
        } catch(const char* msg) {
            // std::cout << "attempts: " << attempts << ", tried_active_vlbs.size()=" << tried_active_vlbs.size() << std::endl;
        }
    }
}

// In a flow that does not respect VLBs, we must pick which will be set back to 0 and which will be set to at least min. qtty.
// Store that info in a map : vlb_edge -> p, where p is the probability that vlb_edge will be set to min. qtty.
std::unordered_map<edge_key, float> compute_new_active_vlbs_p_map(const Network &network, const Flow &flow) {
    auto wannabe_vlbs = network.detect_wannabe_active_vlbs(flow);
    std::unordered_map<edge_key, float> p_map;
    for (auto edge : wannabe_vlbs) {
        // int minimum_quantity = network.minimum_quantities.at(edge);
        int current_value = flow.edge_value(edge);
        auto vertices = get_vertex_keys(edge);
        int minimum_quantity = network.v_minimum_quantities[vertices.first][vertices.second];
        // int current_value = flow.v_values[vertices.first][vertices.second];
        float p = (float) current_value / (float) minimum_quantity; // no need for cutoff above 1
        p_map[edge] = p;
    }
    return p_map;
}

// mutate can change which vlbs are active
std::set<edge_key> pick_new_active_vlbs(const Network &network, std::unordered_map<edge_key, float> active_vlbs_p_map) {
    std::set<edge_key> active_vlbs;
    for (auto edge_p : active_vlbs_p_map) {
        auto edge = edge_p.first;
        auto vertices = get_vertex_keys(edge);
        vertex_key v_from = vertices.first;
        vertex_key v_to = vertices.second;

        // TODO FIXME this should hold at this point
        assert(network.exists_path(v_from, v_to));
        if (!network.exists_path(network.source, v_from) || !network.exists_path(v_to, network.sink)) {
            continue;
        }

        float x = (float) rand() / (float) RAND_MAX;
        if (x < edge_p.second) {
            active_vlbs.insert(edge);
        }
    }
    return active_vlbs;
}


std::pair<std::set<edge_key>, Flow> mutate_throws(const Network &network, const Flow &original, std::set<edge_key> original_active_vlbs, int num_perturbations) {
    int original_value = original.flow_value();
    Flow flow = original.make_copy();

    for (int i = 0; i < num_perturbations; ++i) {
        std::vector<vertex_key> source_to_v_from = find_random_augmenting_path(network, flow, original_active_vlbs, network.source, network.sink, -1, true, true);
        apply_augmenting_path(network, flow, original_active_vlbs, source_to_v_from, -1);
        std::vector<vertex_key> v_to_to_sink = find_random_augmenting_path(network, flow, original_active_vlbs, network.source, network.sink, 1, true, true);
        apply_augmenting_path(network, flow, original_active_vlbs, v_to_to_sink, 1);
    }

    auto vlbs_p_map = compute_new_active_vlbs_p_map(network, flow);
    std::set<edge_key> active_vlbs;
    active_vlbs = pick_new_active_vlbs(network, vlbs_p_map);
    flow = fix_inactive_vlbs(network, flow, active_vlbs);
    flow = fix_active_vlbs(network, flow, active_vlbs);
    flow = fix_flow_value(network, flow, active_vlbs, original_value);

    assert(flow.respects_flow_conservation());
    assert(flow.flow_value() == original_value);
    assert(network.respects_bounds(flow, false));

    return std::make_pair(active_vlbs, flow);
}

std::pair<std::set<edge_key>, Flow> mutate(const Network &network, const Flow &original, std::set<edge_key> original_active_vlbs, int num_perturbations) {
    while (true) {
        try {
            auto mutated = mutate_throws(network, original, original_active_vlbs, num_perturbations);
            return mutated;
        } catch(const char* msg) {
            
        }
    }
}

std::vector<Flow> decompose(const Flow &f, const Network &network) {
    Flow flow_copy = f.make_copy();
    int decompose_into_n_flows = flow_copy.flow_value();
    std::vector<Flow> flow_decomposition;

    for (int i = 0; i < decompose_into_n_flows; ++i) {
        std::queue<vertex_key> to_visit;
        std::set<vertex_key> visited;
        std::map<vertex_key, vertex_key> unitary_path_parents;
        to_visit.push(network.source);
        bool reached_sink = false;
        while (!reached_sink) { // loop until sink is added to unitary_path
            vertex_key visiting = to_visit.front();
            to_visit.pop();
            visited.insert(visiting);
            for (auto next_vertex : flow_copy.outgoing[visiting]) {
                if (visited.find(next_vertex) != visited.end() || flow_copy.edge_value(visiting, next_vertex) < 1) {
                // if (visited.find(next_vertex) != visited.end() || flow_copy.v_values[visiting][next_vertex] < 1) {
                    continue;
                }
                to_visit.push(next_vertex);
                unitary_path_parents[next_vertex] = visiting;
                if (next_vertex == network.sink) {
                    reached_sink = true;
                    break;
                }
            }
        }
        Flow unitary_flow(network.source, network.sink, network.n_nodes, network.max_span_q);
        vertex_key current_vertex = network.sink;
        while (current_vertex != network.source) {
            vertex_key parent = unitary_path_parents[current_vertex];
            unitary_flow.add_edge(parent, current_vertex, 1);
            flow_copy.subtract_from_edge(parent, current_vertex, 1);
            current_vertex = parent;
        }
        flow_decomposition.push_back(unitary_flow);
    }

    return flow_decomposition;
}

std::set<edge_key> decomp_vlbs(const Network &network, std::vector<Flow> &decom) {
    std::set<edge_key> vlbs;
    for (auto & flow : decom) {
        auto wannabe_vlbs = network.detect_wannabe_active_vlbs(flow);
        for (auto edge : wannabe_vlbs) {
            vlbs.insert(edge);
        }
    }
    return vlbs;
}

bool contains_only_active(const Network &network, const Flow &flow, std::set<edge_key> active_vlbs) {
    auto wannabe_vlbs = network.detect_wannabe_active_vlbs(flow);
    for (auto edge : wannabe_vlbs) {
        if (active_vlbs.find(edge) == active_vlbs.end()) {
            return false;
        }
    }
    return true;
}

bool valid_flow(const Network &network, const Flow &flow, int flow_value, bool report) {
    bool respects_flow_conservation = flow.respects_flow_conservation();
    bool correct_value = flow.flow_value() == flow_value;
    bool respects_bounds = network.respects_bounds(flow, report);
    if (!respects_flow_conservation && report) {
        std::cout << "doesn't respect flow conservation" << std::endl;
    }
    if (!correct_value && report) {
        std::cout << "incorrect value: " << flow.flow_value() << ", " << flow_value << std::endl;
    }
    if (!respects_bounds && report) {
        std::cout << "doesn't respect bounds" << std::endl;
    }
    return respects_flow_conservation && correct_value && respects_bounds;
}

// picks a random
std::set<edge_key> joint_random_decom_vlbs(const Network &network, std::vector<Flow> &decom1, std::vector<Flow> &decom2) {
    auto decom1_vlbs = decomp_vlbs(network, decom1);
    auto decom2_vlbs = decomp_vlbs(network, decom2);
    int num_vlbs_1 = decom1_vlbs.size();
    int num_vlbs_2 = decom1_vlbs.size();
    int num_vlbs = (num_vlbs_1 + num_vlbs_2) / 2;

    for (auto edge : decom2_vlbs) {
        decom1_vlbs.insert(edge);
    }

    std::set<edge_key> active_vlbs;

    for (int i = 0; i < num_vlbs; ++i) {
        assert(decom1_vlbs.size() > 0);
        int x = rand() % decom1_vlbs.size(); // get random index
        edge_key selected = *select_random(decom1_vlbs, x); // get the actual index into decom1
        decom1_vlbs.erase(selected);
        active_vlbs.insert(selected);
    }

    return active_vlbs;
}

Flow compose_throws(std::vector<Flow> &decom1, std::vector<Flow> &decom2, std::set<edge_key> active_vlbs, int flow_value,
             const Network &network) {  // ???cubic complexity of the length of the flow worst case
                                        // scenario - reduced to quadratic
    int decomposition_size = decom1.size();
    assert(decomposition_size == decom2.size());
    assert(flow_value == decomposition_size);
    Flow new_flow(network.source, network.sink, network.n_nodes, network.max_span_q);  // by not testing every pair combination of decom1 and decom2, but decom1.size() pairs
    // new_flow.vectorize();
    Flow tmp(network.source, network.sink, network.n_nodes, network.max_span_q);  // alternative would be to implement
    // tmp.vectorize();

    std::set<int> available1; // stores available indices into decom1
    std::set<int> available2; // this avoids the need to resize decom1, decom2 in each step (set operations are O(1) in contrast to vector)

    for (int i = 0; i < decomposition_size; ++i) { // in the beginning, all decomposition elements are available
        if (contains_only_active(network, decom1[i], active_vlbs)) {
            available1.insert(i);
        }
        if (contains_only_active(network, decom2[i], active_vlbs)) {
            available2.insert(i);
        }
    }

    int current_value = new_flow.flow_value();
    while (current_value != flow_value) {
        // if there is a single remaining flow to be added
        if (available1.size() == 0 || available2.size() == 0) {
            std::set<int> *available = &available1;
            std::vector<Flow> *decom = &decom1;
            if (available1.size() == 0) {
                available = &available2;
                decom = &decom2;
            }
            // until all unary flows have been checked, check another one
            if (available->size() == 0) {
                throw "recomposition impossible (1)";
            }
            int x = rand() % available->size(); // get random index
            int selected = *select_random(*available, x); // get the actual index into decom1
            available->erase(selected);
            new_flow.add_flows((*decom)[selected]);
            if (!new_flow.respects_flow_conservation() || !network.respects_upper_bounds(new_flow, false)) {
                new_flow.subtract_flows((*decom)[selected]);
            }
        } else { // if 2 or more unary flows are missing to a full flow
            std::set<std::pair<int, int> > pairs_to_check;
            for (auto first : available1) {
                for (auto second : available2) {
                    pairs_to_check.insert(std::make_pair(first, second));
                }
            }
            bool found = false;
            while (pairs_to_check.size() > 0) {
                int random = rand() % pairs_to_check.size(); // get random index
                auto checking_pair = *select_random(pairs_to_check, random); // get the actual index into decom1
                pairs_to_check.erase(checking_pair);
                int selected1 = checking_pair.first;
                int selected2 = checking_pair.second;

                // check to see if the new flow is valid, if yes reset checked_flows1/2 and remove the
                // unary flows added, if not check the next flow
                new_flow.add_flows(decom1[selected1]);
                new_flow.add_flows(decom2[selected2]);

                if (new_flow.respects_flow_conservation() && network.respects_upper_bounds(new_flow, false)) {
                    available1.erase(selected1);
                    available2.erase(selected2);
                    found = true;
                    break; // redundant but for clarity
                } else {
                    new_flow.subtract_flows(decom1[selected1]);
                    new_flow.subtract_flows(decom2[selected2]);
                }
            }

            if (!found) {
                    throw "recomposition impossible (2)";
            }
        }

        int new_value = new_flow.flow_value();
        current_value = new_value;
    }

    assert(new_flow.flow_value() == flow_value);
    
    // auto active_vlbs = network.detect_wannabe_active_vlbs(new_flow); TODO FIXME check if they are the same as preset ones
    new_flow = fix_active_vlbs(network, new_flow, active_vlbs);
    new_flow = fix_inactive_vlbs(network, new_flow, active_vlbs);
    new_flow = fix_flow_value(network, new_flow, active_vlbs, flow_value);

    assert(valid_flow(network, new_flow, flow_value, true));

    return new_flow;
}

std::pair<std::set<edge_key>, Flow> compose(std::vector<Flow> &decom1, std::vector<Flow> &decom2, int flow_value,
             const Network &network) {  // ???cubic complexity of the length of the flow worst case
    int attempts = 0;
    while (true) {
        ++attempts;
        try {
            auto active_vlbs = joint_random_decom_vlbs(network, decom1, decom2);
            Flow flow = compose_throws(decom1, decom2, active_vlbs, flow_value, network);
            // if (attempts > 1) {
            //     std::cout << "took " << attempts << " attempts" << std::endl;
            // }
            assert(valid_flow(network, flow, flow_value, true));
            return std::make_pair(active_vlbs, flow);
        } catch(const char* msg) {
            
        }
    }
}

std::pair<std::set<edge_key>, Flow> crossover(const Network &network, Flow &f1, Flow &f2, int flow_value) {
    std::vector<Flow> decomposed1 = decompose(f1, network);
    std::vector<Flow> decomposed2 = decompose(f2, network);
    int decomposition_size = decomposed1.size();
    assert(decomposition_size == decomposed2.size());
    assert(flow_value == decomposition_size);
    auto recomposed = compose(decomposed1, decomposed2, flow_value, network);
    // recomposed.second.vectorize();
    assert(valid_flow(network, recomposed.second, flow_value, true));
    return recomposed;
}

void check_difference(const Flow &one, const Flow &two) {
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

// int fitness(const Network &network, const Flow &flow) {
//     return network.total_cost(flow);
// }

int cost(const Network &network, const Flow &flow) {
    return network.total_cost(flow);
}

Solution random_solution(const Network &network, int flow_value) {
    std::set<std::set<edge_key > > tried_active_vlbs;
    auto vlbs_flow = random_admissible_flow(network, flow_value, flow_value / 2, tried_active_vlbs);
    auto active_vlbs = vlbs_flow.first;
    auto flow = vlbs_flow.second;
    assert(valid_flow(network, flow, flow_value, true));
    return Solution(flow, active_vlbs);
}

std::vector<Solution*> initial_generation(const Network &network, int generation_size, int flow_value) {
    std::vector<Solution*> generation;
    std::set<std::set<edge_key > > tried_active_vlbs;
    for (int i = 0; i < generation_size; ++i) {
        std::cout << i << std::endl;
        auto vlbs_flow = random_admissible_flow(network, flow_value, flow_value / 2, tried_active_vlbs);
        auto active_vlbs = vlbs_flow.first;
        auto flow = vlbs_flow.second;
        assert(valid_flow(network, flow, flow_value, true));
        auto solution = new Solution(flow, active_vlbs);
        generation.push_back(solution);
    }
    return generation;
}

void evaluate_solution(const Network &network, Solution *solution) {
    int flow_cost = cost(network, solution->flow);
    solution->cost = flow_cost;
}

void evaluate_generation(const Network &network, std::vector<Solution*> &generation) {
    for (int i = 0; i < generation.size(); ++i) {
        evaluate_solution(network, generation[i]);
    }
}

int pick_best_of(int best_of, int generation_size) {
    int current_best = generation_size - 1;
    for (int i = 0; i < best_of; ++i) {
        int x = rand() % generation_size;
        current_best = min(current_best, x);
    }
    return current_best;
}

void replace_in_generation(std::vector<Solution*> &generation, Solution *solution, int i) {
    delete generation[i];
    generation[i] = solution;
}

void replace_first_worse_or_last(const Network &network, Solution *solution, std::vector<Solution*> &generation) {
    int i;
    for (i = 0; i < generation.size() - 1 && solution->cost > generation[i]->cost; ++i) {}
    replace_in_generation(generation, solution, i);
}
// TODO replace_similar?

void mutate_random(const Network &network, std::vector<Solution*> &generation, int num_permutations) {
    int x = rand() % generation.size();
    auto active_vlbs_flow = mutate(network, generation[x]->flow, generation[x]->active_vlbs, num_permutations);
    Solution *solution = new Solution(active_vlbs_flow.second, active_vlbs_flow.first);
    evaluate_solution(network, solution);
    replace_in_generation(generation, solution, x);
}

bool compare(Solution* a, Solution* b) { 
    return a->cost < b->cost;
}

Solution ga_solver(const Network &network, const SolverParameters &sp, int report_every) {
    int num_steps = sp.num_steps;
    int generation_size = sp.generation_size;
    int flow_value = sp.flow_value;
    assert(generation_size % 2 == 0);
    std::vector<Solution*> current_generation = initial_generation(network, generation_size, sp.flow_value);
    std::cout << "generated initial generation..." << std::endl;
    Solution best_solution = current_generation[0]->make_copy();

    for (int i = 0; i < num_steps; ++i) {
        evaluate_generation(network, current_generation);
        std::sort(current_generation.begin(), current_generation.end(), compare); // TODO FIXME replace with assertion that it's sorted
        if (current_generation[0]->cost <= best_solution.cost || best_solution.cost == -1) {
            best_solution = current_generation[0]->make_copy();
        }        
        if (report_every > 0) {
            if (i % report_every == 0) {
                std::cout << "step " << i << "/" << num_steps << ", best cost: " << best_solution.cost << ", best in generation: " << current_generation[0]->cost << std::endl;
            }
        }
        int first_parent = pick_best_of(sp.best_of, generation_size);
        int second_parent = pick_best_of(sp.best_of, generation_size);
        auto child_vlbs_flow = crossover(network, current_generation[first_parent]->flow, current_generation[second_parent]->flow, flow_value);
        Solution *child = new Solution(child_vlbs_flow.second, child_vlbs_flow.first);
        evaluate_solution(network, child);
        replace_first_worse_or_last(network, child, current_generation);
        mutate_random(network, current_generation, sp.num_perturbations);
    }

    return best_solution;
}
