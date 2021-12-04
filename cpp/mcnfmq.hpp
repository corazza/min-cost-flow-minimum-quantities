/* - representation of MCNF-MQ problem instance
 *    - deserialize from JSON
 *    - serialize to JSON
 * - representation of flows
 */

#ifndef MCNFMQ_H
#define MCNFMQ_H

#include <unordered_map>

#include "json.hpp"
using json = nlohmann::json;

// needed because C++ doesn't have default implementations for pair<int, int> keys in a hashmap ...
typedef std::uint32_t vertex_key;
typedef std::uint64_t edge_key;
// ... so this hack is the best solution (https://stackoverflow.com/a/39690912/924313)
inline edge_key get_edge_key(vertex_key v1, vertex_key v2) {
    return (std::uint64_t)v1 << 32 | (std::uint64_t)v2;  // edge 64b = (v1 32b, v2 32b)
}

struct Mcnfmq {
    unsigned int n_nodes;
    vertex_key source;
    vertex_key sink;
    unsigned int flow_value;  // _desired_ flow value

    std::unordered_map<edge_key, int> costs;
    std::unordered_map<edge_key, int> capacities;
    std::unordered_map<edge_key, int> minimum_quantities;

    Mcnfmq() {}
    Mcnfmq(unsigned int n_nodes, vertex_key source, vertex_key sink, unsigned int flow_value)
        : n_nodes(n_nodes), source(source), sink(sink), flow_value(flow_value) {}

    // NLOHMANN_DEFINE_TYPE_INTRUSIVE(Mcnfmq, n_nodes, source, sink, costs, capacities,
    //    minimum_quantities)
};

void to_json(json& j, const Mcnfmq& instance);
void from_json(const json& j, Mcnfmq& instance);

typedef std::unordered_map<edge_key, int> Flow;

// assumes f respects flow conservation (sum inputs = sum outputs on each node, etc.)
// returns -1 if:
//  - f doesn't match mcnfmq (f contains edge that doesn't exist in mcnfmq)
//  - f doesn't respect variable lower bounds or upper bounds (capacities)
// returns the sum of inputs to the sink node otherwise
int flow_value(Flow& f, Mcnfmq& mcnfmq);

#endif
