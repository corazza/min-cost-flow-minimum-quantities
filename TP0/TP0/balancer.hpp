#ifndef BALANCER_H
#define BALANCER_H

#include "keys.hpp"
#include "network.hpp"

#include <set>
#include <unordered_map>

struct CapacityData {
    std::unordered_map<vertex_key, int> vertex_effective_capacity;
    std::unordered_map<edge_key, int> edge_effective_capacity;

    void CapacityData::compute_effective_capacities(Network &network, int sink_flow_value);

    int remaining_capacity(Network &network, Flow &flow, vertex_key v);
    int remaining_capacity(Network &network, Flow &flow, vertex_key v_from, vertex_key v_to);

private:
    int effective_capacity(vertex_key v);
    int effective_capacity(vertex_key v_from, vertex_key v_to);
};

struct BalancerData {
    bool balanced();
    vertex_key get_unbalanced_vertex();
    void modify_balance(vertex_key v_from, int by);
    int sending(vertex_key v_from);

    BalancerData(vertex_key v_from, int send);

private:
    std::unordered_map<vertex_key, int> has_to_send;
    bool is_balanced;
    bool balanced_dirty;
    vertex_key proof_unbalanced;

    bool find_proof();
};

void propagate(Network &network, Flow &flow, vertex_key v_from, int send, bool forward);

#endif
