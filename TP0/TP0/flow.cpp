#include <iostream>

#include "flow.hpp"

void Flow::empty_flow() {
    this->values.clear();
    this->outgoing.clear();
    this->incoming.clear();
}

void Flow::add_edge(vertex_key v_from, vertex_key v_to, int value) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    this->values[edge] = value;

    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

int Flow::current_value(vertex_key v_from, vertex_key v_to) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    if (this->values.find(edge) == this->values.end()) {
        return 0;
    } else {
        return this->values[edge];
    }
}

bool Flow::exists_edge(const edge_key& edge) {
    if(this->values.find(edge) == this->values.end()) return true;
    return false;
}

int Flow::recompute_value(Network &network) {
    int value = 0;
    for (auto v_from : this->incoming[network.sink]) {
        edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)network.sink);
        value+=this->values[edge];
    }
    return value;
}


int Flow::remove_edge(vertex_key v_from, vertex_key v_to) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    if (this->values.find(edge) == this->values.end() || this->values[edge] == 0) {
        return 0;
    } else {
        int removing = this->values[edge];
        this->values.erase(edge);
        this->outgoing[v_from].erase(v_to);
        this->incoming[v_to].erase(v_from);
        return removing;
    }
}

void Flow::add_to_edge(vertex_key v_from, vertex_key v_to, int value) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->exists_edge(edge)) {
        this->add_edge(v_from, v_to, value);
    } else {
        this->values[edge] += value;
    }
}

void Flow::subtract_from_edge(vertex_key v_from, vertex_key v_to, int value) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->values[edge] > value) this->values[edge] -= value;
    else
        this->remove_edge(v_from, v_to);
}

void Flow::add_flows(const Flow& f) {
    for (auto it1 = f.outgoing.begin(); it1 != f.outgoing.end(); ++it1) {
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
            edge_key edge = get_edge_key(it1->first, *it2);
            int value = f.values.at(edge);
            this->add_to_edge(it1->first, *it2, value);
        }
    }
}

void Flow::subtract_flows(const Flow& f) {
    for (auto it1 = f.outgoing.begin(); it1 != f.outgoing.end(); ++it1) {
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
            edge_key edge = get_edge_key(it1->first, *it2);
            int value = f.values.at(edge);
            this->subtract_from_edge(it1->first, *it2, value);
        }
    }
}

void Flow::print() {
    for (auto edge : this->values) {
        auto vs = get_vertex_keys(edge.first);
        std::cout << vs.first << "->" << vs.second << ": " << edge.second << std::endl;
    }
}

Flow Flow::make_copy() const {
    Flow result(this->value);

    result.values = this->values;
    result.outgoing = this->outgoing;
    result.incoming = this->incoming;

    return result;
}
