#include <iostream>
#include <cassert>

#include "flow.hpp"

void Flow::empty_flow() {
    this->values.clear();
    this->outgoing.clear();
    this->incoming.clear();
    this->value_dirty = true;
}

int Flow::edge_value(vertex_key v_from, vertex_key v_to) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    if (this->values.find(edge) == this->values.end()) {
        return 0;
    } else {
        return this->values[edge];
    }
}

int Flow::incoming_value(vertex_key v_to) {
    int incoming_sum = 0;
    for (auto v_from : this->incoming[v_to]) {
        incoming_sum += this->edge_value(v_from, v_to);
    }
    return incoming_sum;
}

int Flow::outgoing_value(vertex_key v_from) {
    int outgoing_sum = 0;
    for (auto v_to : this->outgoing[v_from]) {
        outgoing_sum += this->edge_value(v_from, v_to);
    }
    return outgoing_sum;
}

int Flow::vertex_value(vertex_key v_from) {
    int incoming_value = this->incoming_value(v_from);
    int outgoing_value = this->outgoing_value(v_from);
    assert(incoming_value == outgoing_value);
    return incoming_value;
}

bool Flow::exists_edge(const edge_key& edge) {
    if(this->values.find(edge) == this->values.end()) return true;
    return false;
}

bool Flow::respects_flow_conservation() {
    for (int i = this->source+1; i < this->sink; ++i) {
        int incoming_sum = 0;
        int outgoing_sum = 0;
        for (auto incoming : this->incoming[i]) {
            incoming_sum += this->edge_value(incoming, i);
        }
        for (auto outgoing : this->outgoing[i]) {
            outgoing_sum += this->edge_value(i, outgoing);
        }
        if (incoming_sum != outgoing_sum) {
            return false;
        }
    }
    return true;
}

void Flow::print() {
    for (int i = this->source; i <= this->sink; ++i) {
        if (this->outgoing.find(i) == this->outgoing.end()) {
            continue;
        }
        if (this->outgoing[i].size() == 0) {
            continue;
        }
        std::cout << i << std::endl;
        for (auto outgoing : this->outgoing[i]) {
            std::cout << "  -> " << outgoing << " (" << this->edge_value(i, outgoing) << ")" << std::endl;
        }
    }
}

Flow Flow::make_copy() const {
    Flow result(this->value, this->source, this->sink);

    result.values = this->values;
    result.outgoing = this->outgoing;
    result.incoming = this->incoming;

    return result;
}

int Flow::recompute_value() {
    int value = 0;
    for (auto v_from : this->incoming[this->sink]) {
        edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)this->sink);
        value+=this->values[edge];
    }
    return value;
}

int Flow::flow_value() {
    if (this->value_dirty) {
        this->value = this->recompute_value();
    }
    return this->value;
}

void Flow::add_edge(vertex_key v_from, vertex_key v_to, int value) {
    assert(v_from != v_to);
    this->value_dirty = true;
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);

    this->values[edge] = value;

    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

int Flow::remove_edge(vertex_key v_from, vertex_key v_to) {
    assert(v_from != v_to);
    this->value_dirty = true;
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
    this->value_dirty = true;
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->exists_edge(edge)) {
        this->add_edge(v_from, v_to, value);
    } else {
        this->values[edge] += value;
    }
}

void Flow::subtract_from_edge(vertex_key v_from, vertex_key v_to, int value) {
    assert(v_from != v_to);
    this->value_dirty = true;
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->exists_edge(edge)) {
        if (this->values[edge] > value) {
            this->values[edge] -= value;
        }
        else {
            this->remove_edge(v_from, v_to);
        }
    }
}

void Flow::add_flows(const Flow& f) {
    this->value_dirty = true;
    for (auto it1 = f.outgoing.begin(); it1 != f.outgoing.end(); ++it1) {
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
            edge_key edge = get_edge_key(it1->first, *it2);
            int value = f.values.at(edge);
            this->add_to_edge(it1->first, *it2, value);
        }
    }
}

void Flow::subtract_flows(const Flow& f) {
    this->value_dirty = true;
    for (auto it1 = f.outgoing.begin(); it1 != f.outgoing.end(); ++it1) {
        for (auto it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
            edge_key edge = get_edge_key(it1->first, *it2);
            int value = f.values.at(edge);
            this->subtract_from_edge(it1->first, *it2, value);
        }
    }
}
