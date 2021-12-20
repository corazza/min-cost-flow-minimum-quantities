#ifndef KEYS_H
#define KEYS_H

#include <cstdint>
#include <utility>

// needed because C++ doesn't have default implementations for pair<int, int> keys in a hashmap ...
typedef std::uint32_t vertex_key;
typedef std::uint64_t edge_key;
// ... so this hack is the best solution (https://stackoverflow.com/a/39690912/924313)
inline edge_key get_edge_key(vertex_key v_from, vertex_key v_to) {
    return (std::uint64_t)v_from << 32 | (std::uint64_t)v_to;  // edge 64b = (v1 32b, v2 32b)
}
inline std::pair<vertex_key, vertex_key> get_vertex_keys(edge_key edge) {
    std::uint32_t v_from = (std::uint32_t)(edge >> 32);
    std::uint32_t v_to = (std::uint32_t)(edge & (((std::uint64_t)1 << 32) - 1));
    return std::make_pair((vertex_key)v_from, (vertex_key)v_to);
}

#endif
