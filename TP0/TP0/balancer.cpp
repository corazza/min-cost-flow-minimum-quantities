#include "balancer.hpp"
#include "util.hpp"

#include <queue>

int CapacityData::remaining_capacity(Network &network, Flow &flow, vertex_key v) {
    return flow.vertex_value(v) - this->effective_capacity(v);
}

int CapacityData::remaining_capacity(Network &network, Flow &flow, vertex_key v_from, vertex_key v_to) {
    return flow.edge_value(v_from, v_to) - this->effective_capacity(v_from, v_to);
}

int CapacityData::effective_capacity(vertex_key v) {
    if (this->vertex_effective_capacity.find(v) == this->vertex_effective_capacity.end()) {
        return 0;
    }
    return this->vertex_effective_capacity[v];
}

int CapacityData::effective_capacity(vertex_key v_from, vertex_key v_to) {
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    if (this->edge_effective_capacity.find(edge) == this->edge_effective_capacity.end()) {
        return 0;
    }
    return this->edge_effective_capacity[edge];
}

void CapacityData::compute_effective_capacities(Network &network, int sink_flow_value) {
    for (int i = network.source; i <= network.sink; ++i) {
        this->vertex_effective_capacity[i] = 0;
    }

    this->vertex_effective_capacity[network.sink] = sink_flow_value;

    std::queue<vertex_key> to_visit; // assumes no cycles
    std::set<vertex_key> to_visit_check;

    for (auto &key : network.incoming[network.sink]) {
        to_visit.push(key);
    }

    while (!to_visit.empty()) {
        auto visiting = to_visit.front();
        to_visit.pop();
        to_visit_check.erase(visiting);

        for (auto &key : network.incoming[visiting]) {
            if (to_visit_check.find(key) == to_visit_check.end()) {
                to_visit.push(key);
                to_visit_check.insert(key);
            }
        }

        int outgoing_capacity = 0;
        for (auto &neighbor : network.outgoing[visiting]) {
            edge_key edge = get_edge_key(visiting, neighbor);
            int capacity = network.capacities[edge];
            int neighbor_effective_capacity = this->effective_capacity(neighbor);
            outgoing_capacity += min(capacity, neighbor_effective_capacity);
        }
        this->vertex_effective_capacity[visiting] = outgoing_capacity;
    }

    assert(this->vertex_effective_capacity[network.source] >= sink_flow_value);
}

BalancerData::BalancerData(vertex_key v_from, int send) {
    this->has_to_send[v_from] = send;
    this->balanced_dirty = true;
    this->is_balanced = send == 0;
}

bool BalancerData::find_proof() {
    this->balanced_dirty = false;
    for (auto sending : this->has_to_send) {
        if (sending.second != 0) {
            this->proof_unbalanced = sending.first;
            this->is_balanced = false;
            return false;
        }
    }
    this->is_balanced = true;
    return true;
}

bool BalancerData::balanced() {
    if (this->balanced_dirty) {
        return this->find_proof();
    } else {
        return this->is_balanced;
    }
}

vertex_key BalancerData::get_unbalanced_vertex() {
    if (this->balanced_dirty) {
        this->find_proof();
    }
    assert(!this->is_balanced);
    return this->proof_unbalanced;
}

void BalancerData::modify_balance(vertex_key v_from, int by) {
    if (this->has_to_send.find(v_from) == this->has_to_send.end()) {
        this->has_to_send[v_from] = by;
    } else {
        this->has_to_send[v_from] += by;
    }
    this->balanced_dirty = true;
}

int BalancerData::sending(vertex_key v_from) {
    if (this->has_to_send.find(v_from) == this->has_to_send.end()) {
        this->has_to_send[v_from] = 0;
        return 0;
    } else {
        return this->has_to_send[v_from];
    }
}

std::vector<vertex_key> find_augmenting_path(Network &network, Flow &flow, vertex_key v_from, vertex_key v_to, int value) {
    assert(network.exists_path(v_from, v_to));
    bool forward = v_from < v_to;
    std::set<vertex_key> visited;
    vertex_key visiting;
    std::vector<vertex_key> path;

    do {
        visited.insert(visiting);
        path.push_back(visiting);
        for (auto v_to : network.outgoing[visiting]) {

        }
    } while (visiting != v_to);

    return path;
}

void propagate(Network &network, Flow &flow, std::set<edge_key> active_vlbs, vertex_key v_from, int send, bool forward) {
    int original_value = flow.flow_value();
    CapacityData capacity_data;
    capacity_data.compute_effective_capacities(network, original_value);
    BalancerData balancer_data(v_from, send);

    while (!balancer_data.balanced()) {
        vertex_key v_from = balancer_data.get_unbalanced_vertex();
        // int sending = balancer_data.sending(v_from); always send one
        std::vector<vertex_key> will_accept;
        for (auto v_to : network.outgoing[v_from]) {
            int edge_capacity = capacity_data.remaining_capacity(network, flow, v_from, v_to);
        }
    }
}

// void propagate(FlowNetwork &network, Flow &flow, vertex_key v_from, int send, bool forward) {
//     if (!network.computed_effective_capacities) {
//         network.compute_effective_capacities();
//     }

//     ForwardBalancer vertex_values(network, flow);

//     std::queue<vertex_key> to_visit;

//     to_visit.push(v_from);
//     vertex_values.add(network, v_from, send);
//     bool absorbed = false;


//     // vertex_key visiting = network.source;
//     // std::set<vertex_key> directing_set =
//     //     forward ? network.outgoing[visiting] : network.incoming[visiting];

//     // int n_neighbors = directing_set.size();
//     // std::vector<vertex_key> neighbors;
//     // int max_capacity = 0;
//     // int max_throughput = 0;

//     // for (auto neighbor : directing_set) {
//     //     neighbors.push_back(neighbor);

//     //     vertex_key v_from = forward ? visiting : neighbor;
//     //     vertex_key v_to = forward ? neighbor : visiting;

//     //     int neighbor_capacity = vertex_values.remaining_capacity(network, neighbor);
//     //     int edge_capacity = network.capacity(v_from, v_to);
//     //     int capacity_both = min(neighbor_capacity, edge_capacity);
//     //     std::cout << "neighbor: " << neighbor << ", cap = " << neighbor_capacity << ", edge cap = " << edge_capacity << std::endl;
//     //     if (capacity_both > max_capacity) {
//     //         max_capacity = capacity_both;
//     //     }
//     //     int neighbor_throughput = vertex_values.throughput[neighbor];
//     //     int edge_throughput = flow.edge_value(v_from, v_to);
//     //     int throughput_both = min(neighbor_throughput, edge_throughput);
//     //     if (throughput_both > max_throughput) {
//     //         max_throughput = throughput_both;
//     //     }
//     // }

//     // int a;
//     // std::cout << "sink outgoing capacity: " << max_capacity << std::endl;
//     // std::cout << "send=" << send << std::endl;
//     // std::cin >> a;

//     while (!to_visit.empty()) {
//         auto visiting = to_visit.front();
//         to_visit.pop();
//         if ((visiting == network.network.sink && forward) || (visiting == network.network.source && !forward)) {
//             continue;
//         }

//         int value = vertex_values.take(visiting);
//         if (value == 0) {
//             continue;
//         }

//         std::set<vertex_key> directing_set =
//             forward ? network.network.outgoing[visiting] : network.network.incoming[visiting];

//         int n_neighbors = directing_set.size();
//         std::vector<vertex_key> neighbors;

//         while (value != 0) {
//             int max_capacity = 0;
//             int max_throughput = 0;

//             for (auto neighbor : directing_set) {
//                 neighbors.push_back(neighbor);

//                 vertex_key v_from = forward ? visiting : neighbor;
//                 vertex_key v_to = forward ? neighbor : visiting;

//                 int neighbor_capacity = vertex_values.remaining_capacity(network, neighbor);
//                 int edge_capacity = network.network.capacity(v_from, v_to);
//                 int capacity_both = min(neighbor_capacity, edge_capacity);
//                 if (capacity_both > max_capacity) {
//                     max_capacity = capacity_both;
//                 }
//                 int neighbor_throughput = vertex_values.throughput[neighbor];
//                 int edge_throughput = flow.edge_value(v_from, v_to);
//                 int throughput_both = min(neighbor_throughput, edge_throughput);
//                 if (throughput_both > max_throughput) {
//                     max_throughput = throughput_both;
//                 }
//             }

//             int take_from_sending;
//             if (value > 0) {
//                 // if (max_capacity == 0) {
//                 //     std::cout << "failed when visiting " << visiting << ", value = " << value << std::endl;
//                 //     int max_capacity = 0;
//                 //     int max_throughput = 0;

//                 //     for (auto neighbor : directing_set) {
//                 //         neighbors.push_back(neighbor);

//                 //         vertex_key v_from = forward ? visiting : neighbor;
//                 //         vertex_key v_to = forward ? neighbor : visiting;

//                 //         int neighbor_capacity = vertex_values.remaining_capacity(network, neighbor);
//                 //         int edge_capacity = network.capacity(v_from, v_to);
//                 //         int capacity_both = min(neighbor_capacity, edge_capacity);
//                 //         std::cout << "neighbor: " << neighbor << ", cap = " << neighbor_capacity << ", edge cap = " << edge_capacity << std::endl;
//                 //         if (capacity_both > max_capacity) {
//                 //             max_capacity = capacity_both;
//                 //         }
//                 //         int neighbor_throughput = vertex_values.throughput[neighbor];
//                 //         int edge_throughput = flow.edge_value(v_from, v_to);
//                 //         int throughput_both = min(neighbor_throughput, edge_throughput);
//                 //         if (throughput_both > max_throughput) {
//                 //             max_throughput = throughput_both;
//                 //         }
//                 //     }
//                 // }
//                 assert(max_capacity > 0);
//                 take_from_sending = min(value, (rand() % max_capacity) + 1);
//                 take_from_sending = 1;
//             } else {
//                 assert(max_throughput > 0);
//                 take_from_sending = max(value, -((rand() % max_throughput) + 1));
//                 take_from_sending = -1;
//             }
//             assert(take_from_sending != 0);
//             value -= take_from_sending;
//             int look_from = rand() % n_neighbors;
//             int i = look_from;
//             while (true) {
//                 auto neighbor = neighbors[i];
//                 vertex_key v_from = forward ? visiting : neighbor;
//                 vertex_key v_to = forward ? neighbor : visiting;
//                 int after_modification = flow.edge_value(v_from, v_to) + take_from_sending;
//                 bool within_edge_limits =
//                     after_modification >= 0 && after_modification <= network.network.capacity(v_from, v_to);

//                 if (take_from_sending > 0) {
//                     if (vertex_values.remaining_capacity(network, neighbor) >= take_from_sending &&
//                         within_edge_limits) {
//                         vertex_values.add(network, neighbor, take_from_sending);
//                         flow.add_to_edge(v_from, v_to, take_from_sending);
//                         to_visit.push(neighbor);
//                         break;
//                     }
//                 } else {
//                     if (vertex_values.throughput[neighbor] >= -take_from_sending &&
//                         within_edge_limits) {
//                         vertex_values.add(network, neighbor, take_from_sending);
//                         flow.add_to_edge(v_from, v_to, take_from_sending);
//                         to_visit.push(neighbor);
//                         break;
//                     }
//                 }

//                 ++i;
//                 if (i == n_neighbors) {
//                     i = 0;
//                 }

//                 if (i == look_from) {
//                     std::cout << "within_edge_limits=" << within_edge_limits << ", after_modification=" << after_modification << ", capacity=" << network.network.capacity(v_from, v_to) << std::endl;
//                     std::cout << "remaining capacity=" << vertex_values.remaining_capacity(network, neighbor) << ", take_from_sending=" << take_from_sending << std::endl;
//                 }
//                 assert(i != look_from);
//             }
//         }
//     }
// }
