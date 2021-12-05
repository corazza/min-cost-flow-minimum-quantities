# metaheuristike

Min Cost Network Flow with Minimum Quantities (MCNFMQ)

**Problem instance:**
- directed graph `G = (V, E)`
- a source `s ∈ V`, a sink `t ∈ V`
- flow value `F ∈ N`
- arc costs `c : R → N_0`
- arc capacities `u : R → N` (upper bound)
- minimum quantities `λ : R → N0` (variable lower bound)

**Task:**
Find a feasible `f : R → N_0` of flow value `F` in `G`
such that the flow cost `SUM[r ∈ R] f(r) * c(r)` is minimized.

## C++ code usage

Everything important for implementing the C++ parts of the algorithm is defined in `cpp/network.hpp`:

- `Network` struct represents an instance of an MCNF-MQ problem
- `Flow` typedef (`std::unordered_map<edge_key, int>`) represents a flow over a network
- `int flow_value(Flow& f, Network& network)` computes total flow of `f` in `network` (unimplemented)
- `int flow_cost(Flow& f, Network& network)` computes cost of `f` in `network` (unimplemented)

## Compiling

- on Windows with MSYS2 or on Linux:

```
g++ -o test_serialization test_serialization.cpp generator.cpp network.cpp --std=c++17 -static-libgcc -static-libstdc++ -Wall
```