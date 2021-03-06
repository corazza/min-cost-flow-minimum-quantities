#include <iostream>
#include <cassert>

#include "flow.hpp"
#include "util.hpp"

void Flow::empty_flow() {
    this->values.clear();
    this->outgoing.clear();
    this->incoming.clear();
    // vectorize();
}

int Flow::edge_value(vertex_key v_from, vertex_key v_to) const {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    return this->edge_value(edge);
}

int Flow::edge_value(edge_key edge) const {
    if (this->values.find(edge) == this->values.end()) {
        return 0;
    } else {
        return this->values.at(edge);
    }
}

// void Flow::vectorize() {
//     v_values.resize(n_nodes, std::vector<int>(n_nodes, 0));

//     for (int i = 0; i < n_nodes; ++i) {
//         int k = min(i+max_span_q, n_nodes);
//         for (int j = i+1; j < k; ++j) {
//             edge_key edge = get_edge_key(i, j);
//             v_values[i][j] = edge_value(edge);
//         }
//     }

//     vectorized = true;
// }

int Flow::incoming_value(vertex_key v_to) const {
    // assert(vectorized);
    if (this->incoming.find(v_to) == this->incoming.end()) {
        return 0;
    }
    int incoming_sum = 0;
    for (auto v_from : this->incoming.at(v_to)) {
        incoming_sum += this->edge_value(v_from, v_to);
        // incoming_sum += v_values[v_from][v_to];
    }
    return incoming_sum;
}

int Flow::outgoing_value(vertex_key v_from) const {
    // assert(vectorized);
    if (this->outgoing.find(v_from) == this->outgoing.end()) {
        return 0;
    }
    int outgoing_sum = 0;
    for (auto v_to : this->outgoing.at(v_from)) {
        outgoing_sum += this->edge_value(v_from, v_to);
        // outgoing_sum += v_values[v_from][v_to];
    }
    return outgoing_sum;
}

// int Flow::vertex_value(vertex_key v_from) const {
//     int incoming_value = this->incoming_value(v_from);
//     int outgoing_value = this->outgoing_value(v_from);
//     assert(incoming_value == outgoing_value);
//     return incoming_value;
// }

bool Flow::exists_edge(const edge_key& edge) {
    if (this->values.find(edge) == this->values.end()) {
        return false;
    } else {
        return true;
    }
}

bool Flow::respects_flow_conservation() const {
    for (int i = this->source+1; i < this->sink; ++i) {
        if (this->incoming_value(i) != this->outgoing_value(i)) {
            this->print();
            std::cout << i << std::endl;
            return false;
        }
    }
    return this->outgoing_value(this->source) == this->incoming_value(this->sink);
}

void Flow::print() const {
    for (int i = this->source; i <= this->sink; ++i) {
        if (this->outgoing.find(i) == this->outgoing.end()) {
            continue;
        }
        if (this->outgoing.at(i).size() == 0) {
            continue;
        }
        std::cout << i << std::endl;
        for (auto outgoing : this->outgoing.at(i)) {
            std::cout << "  -> " << outgoing << " (" << this->edge_value(i, outgoing) << ")" << std::endl;
        }
    }
}

Flow Flow::make_copy() const {
    Flow result(source, sink, n_nodes, max_span_q);

    result.values = this->values;
    result.outgoing = this->outgoing;
    result.incoming = this->incoming;
    // result.v_values = v_values;
    // result.vectorized = vectorized;
    result.max_span_q = max_span_q;

    return result;
}

int Flow::flow_value() const {
    int from_source = this->outgoing_value(this->source);
    int to_sink = this->incoming_value(this->sink);
    assert(from_source == to_sink);
    return from_source;
}

void Flow::add_edge(vertex_key v_from, vertex_key v_to, int value) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    assert(!this->exists_edge(edge));

    this->values[edge] = value;
    // if (vectorized) {
    //     v_values[v_from][v_to] = value;
    // }

    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

void Flow::ensure_edge(vertex_key v_from, vertex_key v_to) {
    assert(v_from != v_to);
    this->outgoing[v_from].insert(v_to);
    this->incoming[v_to].insert(v_from);
}

int Flow::remove_edge(vertex_key v_from, vertex_key v_to) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    assert(this->exists_edge(edge));

    // if (vectorized) {
    //     v_values[v_from][v_to] = 0;
    // }

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
    if (!this->exists_edge(edge)) {
        this->add_edge(v_from, v_to, value);
    } else {
        this->values[edge] += value;
        // if (vectorized) {
        //     v_values[v_from][v_to] += value;
        // }
    }
}

void Flow::subtract_from_edge(vertex_key v_from, vertex_key v_to, int value) {
    assert(v_from != v_to);
    edge_key edge = get_edge_key((vertex_key)v_from, (vertex_key)v_to);
    if (this->exists_edge(edge)) {
        if (this->values[edge] > value) {
            this->values[edge] -= value;
            // if (vectorized) {
            //     v_values[v_from][v_to] -= value;
            // }
        }
        else {
            this->remove_edge(v_from, v_to);
        }
    }
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
