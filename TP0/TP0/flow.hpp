#ifndef FLOW_H
#define FLOW_H

#include "network.hpp"

struct Flow {
    int value;
    std::unordered_map<edge_key, int> values;

    std::unordered_map<vertex_key, std::set<vertex_key> > outgoing;
    std::unordered_map<vertex_key, std::set<vertex_key> > incoming;

    void add_edge(vertex_key v_from, vertex_key v_to, int value);
    int remove_edge(vertex_key v_from, vertex_key v_to);
    void add_to_edge(vertex_key v_from, vertex_key v_to, int value);

    int current_value(vertex_key v_from, vertex_key v_to);

    bool respects_flow_conservation();

    int cost(Network& network);
    int recompute_value(Network& network);

    void print();
    
    Flow make_copy() const;

    Flow(int value) : value(value) {}
};

#endif
