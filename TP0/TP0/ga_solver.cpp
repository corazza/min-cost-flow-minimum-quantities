#include "ga_solver.hpp"
#include "util.hpp"

#include <set>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <stack>

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

        std::set<vertex_key> directing_set = forward ? network.outgoing[visiting] : network.incoming[visiting];

        int n_neighbors = directing_set.size();
        std::vector<vertex_key> neighbors;

        while (value != 0) {
            int max_capacity = 0;
            int max_throughput = 0;
            int max_edge_capacity = 0;
            int max_edge_value = 0;

            for (auto neighbor : directing_set) {
                neighbors.push_back(neighbor);

                vertex_key v_from = forward ? visiting : neighbor;
                vertex_key v_to = forward ? neighbor : visiting;

                int neighbor_capacity = vertex_values.remaining_capacity(network, neighbor);
                int neighbor_throughput = vertex_values.throughput[neighbor];
                int edge_capacity = network.capacity(v_from, v_to);
                int edge_value = flow.current_value(v_from, v_to);
                if (edge_value > max_edge_value) {
                    max_edge_value = edge_value;
                }
                if (edge_capacity > max_edge_capacity) {
                    max_edge_capacity = edge_capacity;
                }
                if (neighbor_capacity > max_capacity) {
                    max_capacity = neighbor_capacity;
                }
                if (neighbor_throughput > max_throughput) {
                    max_throughput = neighbor_throughput;
                }
            }

            max_throughput = min(max_throughput, max_edge_value);
            max_capacity = min(max_capacity, max_edge_capacity);

            assert(max_capacity > 0 || max_throughput > 0);

int take_from_sending;
if (value > 0) {
    take_from_sending = min(value, (rand() % max_capacity) + 1);
}
else {
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
    int after_modification = flow.current_value(v_from, v_to) + take_from_sending;
    bool within_edge_limits = after_modification >= 0 && after_modification <= network.capacity(v_from, v_to);

    if (take_from_sending > 0) {
        if (vertex_values.remaining_capacity(network, neighbor) >= take_from_sending && within_edge_limits) {
            vertex_values.add(network, neighbor, take_from_sending);
            flow.add_to_edge(v_from, v_to, take_from_sending);
            to_visit.push(neighbor);
            break;
        }
    }
    else {
        if (vertex_values.throughput[neighbor] >= -take_from_sending && within_edge_limits) {
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

Flow random_admissible_flow(Network& network) {
    Flow flow(network.flow_value);
    propagate(network, flow, network.source, network.flow_value, true);
    return flow;
}

Flow mutate(Network& network, const Flow& original) {
    int old_value = original.value;
    Flow flow = original.make_copy(); // TODO assert test

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

    int new_value = flow.recompute_value(network);
    assert(new_value == old_value);

    return flow;
}

std::vector<Flow> decompose(const Flow& f, const Network &network) {
    Flow flow_copy = f.make_copy();
    std::vector<Flow> flow_decomposition;

    Flow tmp_flow(1);

    while (flow_copy.value) {
        tmp_flow.empty_flow();

        // find a unitary flow and subtract the flow from the copy

        std::set<vertex_key> visited; // keeps a set of visited states, this should be redundant with the closed nodes set, which structure has the quickest find implementation?
        std::stack<std::vector<vertex_key>> stack; // keeps the nodes yet to be opened; a stack of maps might be a better choice
        std::set<std::vector<vertex_key>> closed_nodes; // keeps all of the states and their parents

        vertex_key parent = 0;
        std::vector<vertex_key> node(2);
        std::vector<vertex_key> tmp(2);
        node[0] = node[1] = 0;
        stack.push(node);
        visited.insert(node[0]);
        closed_nodes.insert(node);
        
        do{ // find a valid unitary flow
            tmp = stack.top();
            if (visited.find(node[0]) == visited.end()) {
                for (auto it = flow_copy.outgoing[tmp[0]].begin(); it != flow_copy.outgoing[tmp[0]].end(); ++it) {
                    tmp[0] = *it;
                    tmp[1] = parent;
                    stack.push(tmp);
                    visited.insert(tmp[0]);
                    closed_nodes.insert(tmp);
                }
            }
            stack.pop();

        } while (tmp[0] != network.n_nodes);

        node = tmp;

        do { // make the path into a flow and subtract the flow from flow_copy
            tmp_flow.add_edge(node[1], node[0], 1);
            tmp_flow.subtract_from_edge(node[1], node[0], 1);

            auto it = closed_nodes.begin();
            while((*it)[0] != node[1]) ++it; // find the parent node in the closed_nodes set
            node = *it;
        } while (node[1] != 0);
        
        flow_decomposition.push_back(tmp_flow.make_copy());

        --flow_copy.value;
    }

    return flow_decomposition;
}

Flow compose(std::vector<Flow> &decom1, std::vector<Flow> &decom2, const Network &network) { // ???cubic complexity of the length of the flow worst case scenario - reduced to quadratic
    Flow new_flow(0);                                                                        // by not testing every pair combination of decom1 and decom2, but decom1.size() pairs
    Flow tmp(0);                                                                             // alternative would be to implement 
    int random1, random2;
    bool end = false;
    bool backtrack;
    std::vector<bool> checked_flows1(decom1.size(), 0), checked_flows2(decom1.size(), 0);


    while (new_flow.value < network.flow_value && !end) {
        // if there is a single remaining flow to be added
        if (new_flow.value == network.flow_value - 1) {
            //until all unary flows have been checked, check another one
            while (!(decom1.empty()) && !end) {
                random1 = rand() % decom1.size();
                new_flow.add_flows(decom1[random1]);

                if (new_flow.respects_flow_conservation()) end = true; // effectively ends the call to the funtion and returns the flow
                else {
                    new_flow.subtract_flows(decom1[random1]);
                    decom1.erase(decom1.begin() + random1);
                }
            }
            // if there is no valid fill for the flow, ?in theory shouldnt happen
            if (!end) {
                new_flow.empty_flow();
                return new_flow;
            }
        }
        //if 2 or more unary flows are missing to a full flow
        else {
            backtrack = false;
            random1 = rand() % decom1.size();
            random2 = rand() % decom2.size();

            //find unchecked unary flows
            while (random1 > -1) {
                if (checked_flows1[random1]) {
                    if (!backtrack && random1 != decom1.size() - 1) ++random1;
                    else {
                        if (random1 == decom1.size()) backtrack = true;
                        else
                            --random1;
                    }
                }
            }

            while (random2 > -1) {
                if (checked_flows2[random2]) {
                    if (!backtrack && random2 != decom2.size() - 1) ++random2;
                    else {
                        if (random2 == decom2.size()) backtrack = true;
                        else
                            --random2;
                    }
                }
            }
            //if all unary flows have been tested and none fit, ?in theory shouldnt happen
            if (random1 == -1 || random2 == -1) {
                end = true;
                new_flow.empty_flow();
            }

            //check to see if the new flow is valid, if yes reset checked_flows1/2 and remove the unary flows added, if not check the next flow
            if (!end) {
                new_flow.add_flows(decom1[random1]);
                new_flow.add_flows(decom2[random2]);

                if (new_flow.respects_flow_conservation()) {
                    decom1.erase(decom1.begin() + random1);
                    decom2.erase(decom2.begin() + random2);

                    checked_flows1.assign(decom1.size(), 0);
                    checked_flows2.assign(decom2.size(), 0);
                    new_flow.value += 2;
                }
                else {
                    checked_flows1[random1] = true;
                    checked_flows2[random2] = true;
                }
            }
        }
        
    }

    return new_flow;
}