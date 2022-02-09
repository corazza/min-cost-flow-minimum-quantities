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


std::vector<vertex_key> find_random_augmenting_path(const Network &network, Flow &flow, std::set<edge_key> active_vlbs, vertex_key v_start, vertex_key v_end, int value, bool ignore_active_vlbs) {
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
                bool is_vlb = network.vlbs.find(edge) != network.vlbs.end();
                bool is_active_vlb = active_vlbs.find(edge) != active_vlbs.end();
                assert(!is_active_vlb || is_vlb);
                int minimum_quantity = network.minimum_quantities.at(edge);
                int current_value = flow.edge_value(visiting, neighbor);
                int new_value = current_value + value;

                bool within_bounds = new_value <= network.capacity(visiting, neighbor);
                within_bounds &= new_value >= 0;

                if (is_active_vlb) {
                    within_bounds &= (new_value >= minimum_quantity) || ignore_active_vlbs;
                } else if (is_vlb) {
                    within_bounds &= new_value == 0;
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
        current_vertex = next_vertex;
    }
}

std::set<edge_key> random_active_vlbs(const Network &network, int up_to_flow_value) {
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

// should be called before fix_flow_value
Flow fix_vlbs(const Network &network, const Flow &original, std::set<edge_key> active_vlbs) {
    Flow flow = original.make_copy();

    for (auto edge : active_vlbs) {
        int minimum_quantity = network.minimum_quantities.at(edge);
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
    assert(network.respects_bounds(flow));

    return flow;
}

// assumes vlbs are fixed and bounds are respected
Flow fix_flow_value(const Network &network, const Flow &original, std::set<edge_key> active_vlbs, int flow_value) {
    assert(network.respects_bounds(original));
    Flow flow = original.make_copy();

    int original_value = original.flow_value();
    int debt = flow_value - original_value;
    if (debt == 0) {
        return original;
    }

    int increment = debt > 0 ? 1 : -1;
    int steps = abs(debt);

    for (int i = 0; i < steps; ++i) {
        std::vector<vertex_key> source_to_sink = find_random_augmenting_path(network, flow, active_vlbs, network.source, network.sink, increment, false);
        apply_augmenting_path(network, flow, active_vlbs, source_to_sink, increment);
    }

    assert(flow.respects_flow_conservation());
    assert(flow.flow_value() == flow_value);

    return flow;
}

Flow random_admissible_flow_throws(const Network &network, int flow_value, std::set<edge_key> active_vlbs) {
    Flow flow(network.source, network.sink);
    flow = fix_vlbs(network, flow, active_vlbs);
    flow = fix_flow_value(network, flow, active_vlbs, flow_value);
    assert(flow.respects_flow_conservation());
    assert(flow.flow_value() == flow_value);
    assert(network.respects_bounds(flow));
    return flow;
}

// TODO FIXME is the reason it's necessary to try/catch because the network can have dead-end active vlbs?
std::pair<std::set<edge_key>, Flow> random_admissible_flow(const Network &network, int flow_value, int up_to_value) {
    int attempts = 0;
    while (true) {
        ++attempts;
        try {
            std::set<edge_key> active_vlbs = random_active_vlbs(network, up_to_value);
            Flow flow = random_admissible_flow_throws(network, flow_value, active_vlbs);
            return std::make_pair(active_vlbs, flow);
            if (attempts > 1) {
                std::cout << "took " << attempts << " attempts" << std::endl;
            }
        } catch(const char* msg) {
            
        }
    }
}

Flow mutate(const Network &network, const Flow &original, std::set<edge_key> active_vlbs, int num_perturbations) {
    int original_value = original.flow_value();
    Flow flow = original.make_copy();

    for (int i = 0; i < num_perturbations; ++i) {
        std::vector<vertex_key> source_to_v_from = find_random_augmenting_path(network, flow, active_vlbs, network.source, network.sink, -1, false); // TODO FIXME
        apply_augmenting_path(network, flow, active_vlbs, source_to_v_from, -1);
        std::vector<vertex_key> v_to_to_sink = find_random_augmenting_path(network, flow, active_vlbs, network.source, network.sink, 1, false); // TODO FIXME
        apply_augmenting_path(network, flow, active_vlbs, v_to_to_sink, 1);        
    }

    flow = fix_vlbs(network, flow, active_vlbs); // TODO FIXME detect new vlbs
    flow = fix_flow_value(network, flow, active_vlbs, original_value);

    assert(flow.respects_flow_conservation());
    assert(flow.flow_value() == original_value);
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

Flow compose_throws(std::vector<Flow> &decom1, std::vector<Flow> &decom2, std::set<edge_key> active_vlbs,
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
        if (contains_only_active(network, decom1[i], active_vlbs)) {
            available1.insert(i);
        }
        if (contains_only_active(network, decom2[i], active_vlbs)) {
            available2.insert(i);
        }
    }

    std::cout << "available1=" << available1.size() << ", available2=" << available2.size() << std::endl;

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
            while (true) {
                assert(available->size() > 0);
                int x = rand() % available->size(); // get random index
                int selected = *select_random(*available, x); // get the actual index into decom1
                available->erase(selected);
                new_flow.add_flows((*decom)[selected]);

                if (new_flow.respects_flow_conservation() && network.respects_upper_bounds(new_flow))
                    return new_flow;
                else {
                    new_flow.subtract_flows((*decom)[selected]);
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

                if (new_flow.respects_flow_conservation() && network.respects_upper_bounds(new_flow)) {
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

    // auto active_vlbs = network.detect_wannabe_active_vlbs(new_flow); TODO FIXME check if they are the same as preset ones
    std::cout << "active c: " << active_vlbs.size() << std::endl;
    new_flow = fix_vlbs(network, new_flow, active_vlbs);
    std::cout << "fixed vlbs" << std::endl;
    new_flow = fix_flow_value(network, new_flow, active_vlbs, flow_value);
    std::cout << "fixed flow value" << std::endl;

    assert(new_flow.respects_flow_conservation());
    assert(new_flow.flow_value() == flow_value);
    assert(network.respects_bounds(new_flow));

    return new_flow;
}

std::pair<std::set<edge_key>, Flow> compose(std::vector<Flow> &decom1, std::vector<Flow> &decom2,
             const Network &network) {  // ???cubic complexity of the length of the flow worst case
    int attempts = 0;
    while (true) {
        ++attempts;
        try {
            auto active_vlbs = joint_random_decom_vlbs(network, decom1, decom2);
            Flow flow = compose_throws(decom1, decom2, active_vlbs, network);
            return std::make_pair(active_vlbs, flow);
            if (attempts > 1) {
                std::cout << "took " << attempts << " attempts" << std::endl;
            }
        } catch(const char* msg) {
            
        }
    }
}
