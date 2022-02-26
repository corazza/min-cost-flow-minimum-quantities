#ifndef FLOW_H
#define FLOW_H

#include "keys.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include <set>
#include <unordered_map>

struct Flow {
    vertex_key source;
    vertex_key sink;
    unsigned int n_nodes;
    unsigned int max_span_q;

    std::unordered_map<edge_key, int> values;

    std::unordered_map<vertex_key, std::set<vertex_key> > outgoing;
    std::unordered_map<vertex_key, std::set<vertex_key> > incoming;

    // bool vectorized;
    // std::vector<std::vector<int> > v_values;

    // void vectorize();

    void empty_flow();  // makes an empty flow
    void add_edge(vertex_key v_from, vertex_key v_to, int value);
    void ensure_edge(vertex_key v_from, vertex_key v_to);
    int remove_edge(vertex_key v_from, vertex_key v_to);
    void add_to_edge(vertex_key v_from, vertex_key v_to, int value);
    void subtract_from_edge(vertex_key v_from, vertex_key v_to, int value);

    int edge_value(vertex_key v_from, vertex_key v_to) const;
    int edge_value(edge_key) const;
    int outgoing_value(vertex_key v_from) const;
    int incoming_value(vertex_key v_from) const;
    // int vertex_value(vertex_key v_from) const;
    int flow_value() const;

    bool respects_flow_conservation() const;
    bool exists_edge(const edge_key& edge);  // returns true if edge exists in flow this

    void add_flows(const Flow& f);       // destructively adds flow f to this
    void subtract_flows(const Flow& f);  // destructively subracts flow f from flow this

    void print() const ;

    Flow make_copy() const;

    Flow(vertex_key source, vertex_key sink, unsigned int n_nodes, unsigned int max_span_q)
        : source(source), sink(sink), n_nodes(n_nodes), max_span_q(max_span_q) {} // vectorized(false),
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Flow, source, sink, n_nodes, values, outgoing, incoming);

#endif
