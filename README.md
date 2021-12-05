# Min Cost Network Flow with Minimum Quantities

**Problem instance:**
- directed graph `G = (V, E)`
- a source `s ∈ V`, a sink `t ∈ V`
- flow value `F ∈ N`
- arc costs `c : E → N_0`
- arc capacities `u : E → N` (upper bound)
- minimum quantities `λ : E → N_0` (variable lower bound)

**Task:**
Find a feasible `f : E → N_0` of flow value `F` in `G`
such that the flow cost `SUM[e ∈ E] f(e) * c(e)` is minimized.

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