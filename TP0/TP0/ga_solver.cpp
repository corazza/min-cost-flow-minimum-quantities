#include "ga_solver.hpp"
#include "util.hpp"
// #include "balancer.hpp"

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>
#include <algorithm>


std::vector<vertex_key> find_random_augmenting_path(Network &network, Flow &flow, std::set<edge_key> active_vlbs, vertex_key v_start, vertex_key v_end, int value, bool ignore_mqs) {
    assert(network.exists_path(v_start, v_end));
    std::set<vertex_key> blacklisted;
    std::unordered_map<vertex_key, vertex_key> parents;
    vertex_key visiting = v_start;

    while (visiting != v_end) {
        std::set<vertex_key> neighbors = network.outgoing[visiting];
        std::vector<vertex_key> suitable_neighbors;
        for (auto neighbor : neighbors) {
            if (blacklisted.find(neighbor) != blacklisted.end()) {
                continue;
            }
            auto edge = get_edge_key(visiting, neighbor);
            bool is_vlb = network.vlbs.find(edge) != network.vlbs.end();
            bool is_active_vlb = active_vlbs.find(edge) != active_vlbs.end();
            assert(!is_active_vlb || is_vlb);
            int minimum_quantity = network.minimum_quantities[edge];
            int current_value = flow.edge_value(visiting, neighbor);
            int new_value = current_value + value;

            bool within_bounds = new_value <= network.capacity(visiting, neighbor);
            within_bounds &= new_value >= 0;

            if (is_active_vlb) {
                within_bounds &= (new_value >= minimum_quantity) || ignore_mqs;
            } else if (is_vlb) {
                within_bounds &= new_value == 0;
            }

            if (within_bounds) {
                suitable_neighbors.push_back(neighbor);
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

void apply_augmenting_path(Network &network, Flow &flow, std::set<edge_key> active_vlbs, std::vector<vertex_key> &path, int value) {
    vertex_key current_vertex = path[0];
    for (int i = 1; i < path.size(); ++i) {
        vertex_key next_vertex = path[i];
        edge_key edge = get_edge_key(current_vertex, next_vertex);
        flow.add_to_edge(current_vertex, next_vertex, value);
        current_vertex = next_vertex;
    }
}

std::set<edge_key> random_active_vlbs(Network &network, int up_to_flow_value) {
    std::set<edge_key> active_vlbs;
    std::vector<edge_key> vlbs(network.vlbs.begin(), network.vlbs.end());
    int num_vlbs = vlbs.size();
    int added = 0;
    for (int i = 0; i < num_vlbs; ++i) {
        int x_i = rand() % num_vlbs;
        auto edge = vlbs[x_i];
        if (active_vlbs.find(edge) == active_vlbs.end()) {
            auto vertices = get_vertex_keys(edge);
            vertex_key v_from = vertices.first;
            vertex_key v_to = vertices.second;
            int minimum_quantity = network.minimum_quantities.at(edge);
            added += minimum_quantity;
            if (added > up_to_flow_value) {
                break;
            }
            active_vlbs.insert(edge);
        }
    }
    return active_vlbs;
}

Flow random_admissible_flow(Network &network, int flow_value, std::set<edge_key> active_vlbs) {
    Flow flow(network.source, network.sink);

    for (auto edge : active_vlbs) {
        int minimum_quantity = network.minimum_quantities[edge];
        auto vertices = get_vertex_keys(edge);
        vertex_key v_from = vertices.first;
        vertex_key v_to = vertices.second;
        int current_value = flow.edge_value(v_from, v_to);
        int debt = minimum_quantity - current_value;
        for (int i = 0; i < debt; ++i) {
            std::vector<vertex_key> source_to_v_from = find_random_augmenting_path(network, flow, active_vlbs, network.source, v_from, 1, true);
            apply_augmenting_path(network, flow, active_vlbs, source_to_v_from, 1);
            std::vector<vertex_key> v_to_to_sink = find_random_augmenting_path(network, flow, active_vlbs, v_to, network.sink, 1, true);
            apply_augmenting_path(network, flow, active_vlbs, v_to_to_sink, 1);
            flow.add_to_edge(v_from, v_to, 1);
        }
    }

    assert(flow.respects_flow_conservation());

    int current_value = flow.outgoing_value(flow.source);
    int debt = flow_value - current_value;
    assert(debt >= 0);
    for (int i = 0; i < debt; ++i) {
        std::vector<vertex_key> source_to_sink = find_random_augmenting_path(network, flow, active_vlbs, network.source, network.sink, 1, false);
        apply_augmenting_path(network, flow, active_vlbs, source_to_sink, 1);
    }

    assert(flow.respects_flow_conservation());
    assert(flow.flow_value() == flow_value);
    assert(network.respects_bounds(flow));

    return flow;
}

std::pair<std::set<edge_key>, Flow> random_admissible_flow(Network &network, int flow_value, int up_to_value) {
    int attempts = 0;
    while (true) {
        ++attempts;
        try {
            std::set<edge_key> active_vlbs = random_active_vlbs(network, up_to_value);
            Flow flow = random_admissible_flow(network, flow_value, active_vlbs);
            return std::make_pair(active_vlbs, flow);
            if (attempts > 1) {
                std::cout << "took " << attempts << " attempts" << std::endl;
            }
        } catch(const char* msg) {
            
        }
    }
}

Flow mutate(Network &network, const Flow &original, std::set<edge_key> active_vlbs, int num_perturbations) {
    Flow flow = original.make_copy();

    for (int i = 0; i < num_perturbations; ++i) {
        std::vector<vertex_key> source_to_v_from = find_random_augmenting_path(network, flow, active_vlbs, network.source, network.sink, -1, false);
        apply_augmenting_path(network, flow, active_vlbs, source_to_v_from, -1);
        std::vector<vertex_key> v_to_to_sink = find_random_augmenting_path(network, flow, active_vlbs, network.source, network.sink, 1, true);
        apply_augmenting_path(network, flow, active_vlbs, v_to_to_sink, 1);        
    }

    assert(flow.respects_flow_conservation());
    assert(flow.flow_value() == original.flow_value());
    assert(network.respects_bounds(flow));

    return flow;
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
        Flow unitary_flow(network.source, network.sink);
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

template<typename S>
auto select_random(const S &s, size_t n) {
  auto it = std::begin(s);
  // 'advance' the iterator n times
  std::advance(it, n);
  return it;
}

Flow compose(std::vector<Flow> &decom1, std::vector<Flow> &decom2,
             const Network &network) {  // ???cubic complexity of the length of the flow worst case
                                        // scenario - reduced to quadratic
    int decomposition_size = decom1.size();
    assert(decomposition_size == decom2.size());
    int flow_value = decomposition_size;
    Flow new_flow(network.source, network.sink);  // by not testing every pair combination of decom1 and decom2, but decom1.size() pairs
    Flow tmp(network.source, network.sink);  // alternative would be to implement

    std::set<int> available1; // stores available indices into decom1
    std::set<int> available2; // this avoids the need to resize decom1, decom2 in each step (set operations are O(1) in contrast to vector)

    for (int i = 0; i < decomposition_size; ++i) { // in the beginning, all decomposition elements are available
        available1.insert(i);
        available2.insert(i);
    }

    int current_value = new_flow.flow_value();
    while (current_value != flow_value) {
        std::cout << current_value << " / " << flow_value << std::endl;
        // if there is a single remaining flow to be added
        if (current_value == flow_value - 1) {
            // until all unary flows have been checked, check another one
            while (true) {
                assert(available1.size() > 0);
                int random1 = rand() % available1.size(); // get random index
                int selected1 = *select_random(available1, random1); // get the actual index into decom1
                available1.erase(selected1);
                new_flow.add_flows(decom1[selected1]);

                if (new_flow.respects_flow_conservation() && network.respects_bounds(new_flow))
                    return new_flow;
                else {
                    new_flow.subtract_flows(decom1[random1]);
                }
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

                if (new_flow.respects_flow_conservation() && network.respects_bounds(new_flow)) {
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
                    throw "recomposition impossible";
            }
        }

        int new_value = new_flow.flow_value();
        current_value = new_value;
    }

    return new_flow;
}
