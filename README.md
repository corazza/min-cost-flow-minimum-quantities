# metaheuristike

Min Cost Network Flow with Minimum Quantities (MCNFMQ)

**Network:**
- directed graph `G = (V, E)`
- a source `s ∈ V`, a sink `t ∈ V`
- flow value `F ∈ N`
- arc costs `c : R → N_0`
- arc capacities `u : R → N` (upper bound)
- minimum quantities `λ : R → N0` (variable lower bound)

**Task:**
Find a feasible `f : R → N_0` of flow value `F` in `G`
such that the flow cost `SUM[r ∈ R] f(r) * c(r)` is minimized.

## Compiling

MinGW:

```
g++ -o test_serialization test_serialization.cpp generator.cpp network.cpp --std=c++17 -static-libgcc -static-libstdc++ -Wall
```