#include "ga_solver.hpp"

#include <stdlib.h>

#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "util.hpp"

struct ForwardBalancer {
    std::unordered_map<vertex_key, int> sending;
    std::unordered_map<vertex_key, int> throughput;

    int get(vertex_key v) {
        if (this->sending.find(v) == this->sending.end()) {
            this->sending[v] = 0;
            return 0;
        }
        return this->sending[v];
    }

    int take(vertex_key v) {
        int value = this->get(v);
        this->sending[v] = 0;
        return value;
    }

    void add(Network &network, vertex_key v, int value) {
        if (this->sending.find(v) == this->sending.end()) {
            this->sending[v] = value;
            this->throughput[v] = value;
        } else {
            this->sending[v] += value;
            this->throughput[v] += value;
        }

        assert(throughput[v] <= network.vertex_effective_capacity[v]);
    }

    ForwardBalancer(Network &network, Flow &flow) {
        for (auto edge : flow.values) {
            auto vertices = get_vertex_keys(edge.first);
            auto v_from = vertices.first;
            auto v_to = vertices.second;
            auto value = edge.second;
            this->sending[v_from] = 0;
            this->sending[v_to] = 0;

            if (this->throughput.find(v_to) == this->throughput.end()) {
                this->throughput[v_to] = value;
            } else {
                this->throughput[v_to] += value;
            }
        }

        this->throughput[network.source] = 0;

        for (auto v_to : flow.outgoing[network.source]) {
            auto edge = get_edge_key(network.source, v_to);
            auto value = flow.values[edge];
            this->throughput[network.source] += value;
        }
    }

    int remaining_capacity(Network &network, vertex_key v) {
        if (this->throughput.find(v) == this->throughput.end()) {
            return network.vertex_effective_capacity[v];
        }
        return network.vertex_effective_capacity[v] - this->throughput[v];
    }
};

void propagate(Network &network, Flow &flow, vertex_key v_from, int send, bool forward) {
    if (!network.computed_effective_capacities) {
        network.compute_effective_capacities();
    }

    ForwardBalancer vertex_values(network, flow);

    std::queue<vertex_key> to_visit;

    to_visit.push(v_from);
    vertex_values.add(network, v_from, send);
    bool absorbed = false;

    while (!to_visit.empty()) {
        auto visiting = to_visit.front();
        to_visit.pop();
        if ((visiting == network.sink && forward) || (visiting == network.source && !forward)) {
            continue;
        }

        int value = vertex_values.take(visiting);
        if (value == 0) {
            continue;
        }

        std::set<vertex_key> directing_set =
            forward ? network.outgoing[visiting] : network.incoming[visiting];

        int n_neighbors = directing_set.size();
        std::vector<vertex_key> neighbors;

        while (value != 0) {
            int max_capacity = 0;
            int max_throughput = 0;

            for (auto neighbor : directing_set) {
                neighbors.push_back(neighbor);

                vertex_key v_from = forward ? visiting : neighbor;
                vertex_key v_to = forward ? neighbor : visiting;

                int neighbor_capacity = vertex_values.remaining_capacity(network, neighbor);
                int edge_capacity = network.capacity(v_from, v_to);
                int capacity_both = min(neighbor_capacity, edge_capacity);
                if (capacity_both > max_capacity) {
                    max_capacity = capacity_both;
                }
                int neighbor_throughput = vertex_values.throughput[neighbor];
                int edge_throughput = flow.edge_value(v_from, v_to);
                int throughput_both = min(neighbor_throughput, edge_throughput);
                if (throughput_both > max_throughput) {
                    max_throughput = throughput_both;
                }
            }

            int take_from_sending;
            if (value > 0) {
                if (max_capacity == 0) {
                    std::cout << "failed when visiting " << visiting << ", value = " << value << std::endl;
                    int max_capacity = 0;
                    int max_throughput = 0;

                    for (auto neighbor : directing_set) {
                        neighbors.push_back(neighbor);

                        vertex_key v_from = forward ? visiting : neighbor;
                        vertex_key v_to = forward ? neighbor : visiting;

                        int neighbor_capacity = vertex_values.remaining_capacity(network, neighbor);
                        int edge_capacity = network.capacity(v_from, v_to);
                        int capacity_both = min(neighbor_capacity, edge_capacity);
                        std::cout << "neighbor: " << neighbor << ", cap = " << neighbor_capacity << ", edge cap = " << edge_capacity << std::endl;
                        if (capacity_both > max_capacity) {
                            max_capacity = capacity_both;
                        }
                        int neighbor_throughput = vertex_values.throughput[neighbor];
                        int edge_throughput = flow.edge_value(v_from, v_to);
                        int throughput_both = min(neighbor_throughput, edge_throughput);
                        if (throughput_both > max_throughput) {
                            max_throughput = throughput_both;
                        }
                    }
                }
                assert(max_capacity > 0);
                take_from_sending = min(value, (rand() % max_capacity) + 1);
            } else {
                assert(max_throughput > 0);
                take_from_sending = max(value, -((rand() % max_throughput) + 1));
            }
            assert(take_from_sending != 0);
            value -= take_from_sending;
            int look_from = rand() % n_neighbors;
            int i = look_from;
            while (true) {
                auto neighbor = neighbors[i];
                vertex_key v_from = forward ? visiting : neighbor;
                vertex_key v_to = forward ? neighbor : visiting;
                int after_modification = flow.edge_value(v_from, v_to) + take_from_sending;
                bool within_edge_limits =
                    after_modification >= 0 && after_modification <= network.capacity(v_from, v_to);

                if (take_from_sending > 0) {
                    if (vertex_values.remaining_capacity(network, neighbor) >= take_from_sending &&
                        within_edge_limits) {
                        vertex_values.add(network, neighbor, take_from_sending);
                        flow.add_to_edge(v_from, v_to, take_from_sending);
                        to_visit.push(neighbor);
                        break;
                    }
                } else {
                    if (vertex_values.throughput[neighbor] >= -take_from_sending &&
                        within_edge_limits) {
                        vertex_values.add(network, neighbor, take_from_sending);
                        flow.add_to_edge(v_from, v_to, take_from_sending);
                        to_visit.push(neighbor);
                        break;
                    }
                }

                ++i;
                if (i == n_neighbors) {
                    i = 0;
                }

                assert(i != look_from);
            }
        }
    }
}

Flow random_admissible_flow(Network &network) {
    Flow flow(network.flow_value, network.source, network.sink);
    propagate(network, flow, network.source, network.flow_value, true);
    return flow;
}

Flow mutate(Network &network, Flow &original) {
    int old_value = original.flow_value();
    Flow flow = original.make_copy();  // TODO assert test

    std::vector<edge_key> edge_keys;
    for (auto a : flow.values) {
        edge_keys.push_back(a.first);
    }
    int n_edges = edge_keys.size();
    int removing_i = rand() % n_edges;
    auto vertices = get_vertex_keys(edge_keys[removing_i]);
    auto v_from = vertices.first;
    auto v_to = vertices.second;
    int removing_value = flow.values[edge_keys[removing_i]];
    assert(removing_value > 0);
    propagate(network, flow, v_to, -removing_value, true);
    propagate(network, flow, v_from, -removing_value, false);
    flow.remove_edge(v_from, v_to);
    propagate(network, flow, network.source, removing_value, true);

    int new_value = flow.flow_value();
    assert(new_value == old_value);

    return flow;
}

// std::vector<Flow> decompose(const Flow &f, const Network &network) {
//     Flow flow_copy = f.make_copy();
//     std::vector<Flow> flow_decomposition;

//     Flow tmp_flow(1, network.source, network.sink);

//     while (flow_copy.value) {
//         tmp_flow.empty_flow();

//         // find a unitary flow and subtract the flow from the copy

//         std::set<vertex_key>
//             visited;  // keeps a set of visited states, this should be redundant with the closed
//                       // nodes set, which structure has the quickest find implementation?
//         std::stack<std::vector<vertex_key>>
//             stack;  // keeps the nodes yet to be opened; a stack of maps might be a better choice
//         std::set<std::vector<vertex_key>>
//             closed_nodes;  // keeps all of the states and their parents

//         vertex_key parent = 0;
//         std::vector<vertex_key> node(2);
//         std::vector<vertex_key> tmp(2);
//         node[0] = node[1] = 0;
//         stack.push(node);
//         visited.insert(node[0]);
//         closed_nodes.insert(node);

//         do {  // find a valid unitary flow
//             tmp = stack.top();
//             parent = tmp[0];
//             if (visited.find(node[0]) == visited.end()) {
//                 for (auto it = flow_copy.outgoing[tmp[0]].begin();
//                      it != flow_copy.outgoing[tmp[0]].end(); ++it) {
//                     tmp[0] = *it;
//                     tmp[1] = parent;
//                     stack.push(tmp);
//                     visited.insert(tmp[0]);
//                     closed_nodes.insert(tmp);
//                 }
//             }
//             stack.pop();

//         } while (tmp[0] != network.n_nodes);

//         node = tmp;

//         do {  // make the path into a flow and subtract the flow from flow_copy
//             tmp_flow.add_edge(node[1], node[0], 1);
//             tmp_flow.subtract_from_edge(node[1], node[0], 1);

//             auto it = closed_nodes.begin();
//             while ((*it)[0] != node[1]) ++it;  // find the parent node in the closed_nodes set
//             node = *it;
//         } while (node[1] != 0);

//         flow_decomposition.push_back(tmp_flow.make_copy());

//         --flow_copy.value;
//     }

//     return flow_decomposition;
// }

std::vector<Flow> decompose2(const Flow &f, const Network &network) {
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
                edge_key edge = get_edge_key(visiting, next_vertex);
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
        Flow unitary_flow(1, network.source, network.sink);
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
  std::advance(it,n);
  return it;
}

Flow compose(std::vector<Flow> &decom1, std::vector<Flow> &decom2,
             const Network &network) {  // ???cubic complexity of the length of the flow worst case
                                        // scenario - reduced to quadratic
    int decomposition_size = decom1.size();
    assert(decomposition_size == decom2.size());
    Flow new_flow(
        0, network.source, network.sink);  // by not testing every pair combination of decom1 and decom2, but decom1.size() pairs
    Flow tmp(0, network.source, network.sink);  // alternative would be to implement

    std::set<int> available1; // stores available indices into decom1
    std::set<int> available2; // this avoids the need to resize decom1, decom2 in each step (set operations are O(1) in contrast to vector)

    for (int i = 0; i < decomposition_size; ++i) { // in the beginning, all compositions are available
        available1.insert(i);
        available2.insert(i);
    }

    while (new_flow.flow_value() != network.flow_value) {
        // std::cout << "recomposition value: " << new_flow.flow_value() << std::endl;
        // if there is a single remaining flow to be added
        if (new_flow.flow_value() == network.flow_value - 1) {
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
            std::set<std::pair<int, int> > checked_pairs; // keeps track of pairs we checked together
            while (true) {
                int random1 = rand() % available1.size(); // get random index
                int selected1 = *select_random(available1, random1); // get the actual index into decom1
                int random2 = rand() % available2.size();
                int selected2 = *select_random(available2, random2);                
                std::pair<int, int> selection_pair = std::make_pair(selected1, selected2);
                if (checked_pairs.find(selection_pair) != checked_pairs.end()) { // did we check this pair?
                    continue;
                }
                checked_pairs.insert(std::make_pair(selected1, selected2));

                // check to see if the new flow is valid, if yes reset checked_flows1/2 and remove the
                // unary flows added, if not check the next flow
                new_flow.add_flows(decom1[selected1]);
                new_flow.add_flows(decom2[selected2]);

                if (new_flow.respects_flow_conservation() && network.respects_bounds(new_flow)) {
                    available1.erase(selected1);
                    available2.erase(selected2);
                    // std::cout << "selected (" << selected1 << ", " << selected2 << ")" << std::endl;
                    // decom1[selected1].print();
                    // std::cout << std::endl;
                    // decom2[selected2].print();
                    // std::cout << std::endl;
                    // std::cout << std::endl;
                    break; // redundant but for clarity
                } else {
                    new_flow.subtract_flows(decom1[selected1]);
                    new_flow.subtract_flows(decom2[selected2]);
                }
            }
        }
    }

    return new_flow;
}
