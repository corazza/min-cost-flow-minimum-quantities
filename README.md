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

## Usage

Everything important for implementing the C++ parts of the algorithm is defined in `cpp/network.hpp`:

- `Network` struct represents an instance of an MCNF-MQ problem
- `Flow` typedef (`std::unordered_map<edge_key, int>`) represents a flow over a network
- `int flow_value(Flow& f, Network& network)` computes total flow of `f` in `network` (unimplemented)
- `int flow_cost(Flow& f, Network& network)` computes cost of `f` in `network` (unimplemented)

`Network` objects can be imported from or exported into text files, see `data/exported_network_example.json`.
Doing that provides a language/tool-agnostic way of defining data sets (JSON is easy to import into Python etc.)

This is the (current) structure of an instance (`Network` or exported file):

- `n_nodes`: number of nodes in the network
- `source`: key of the source vertex
- `sink`: key of the sink vertex
- `flow_value`: desired flow value
- `outgoing[vertex_key]`, `incoming[vertex_key]`: for vertex `vertex_key` these are sets of vertices that it's connected to (this is for easier lookups etc.)
- `costs[edge_key]`, `capacities[edge_key]`, `minimum_quantities[edge_key]`: map edges to integers as in the problem definition. 

Utility functions:

- `edge_key get_edge_key(vertex_key v_from, vertex_key v_to)`: the edge `(v_from, v_to)` sadly needs to be wrapped into an `edge_key` object
- `std::pair<vertex_key, vertex_key> get_vertex_keys(edge_key edge)`: `pair.first` is `v_from`, `pair.second` is `v_to`
- `bool Network::exists_path(vertex_key v_from, vertex_key v_to)`: DFS from `v_from` to `v_to`
- `bool Network::exists_edge(vertex_key v_from, vertex_key v_to)`: just a lookup in one of the above maps

## Generator status

- TODO: implement the generator as described in paper (1)
    - this will be the `Network generate_instance_paper_one(ParametersOne p)` function

Input to the generator is at the moment (as fields of `Parameters` defined in `cpp/generator.hpp`):
- `n_nodes`: number of nodes the generated network will have (current generator doesn't utilize them all, working on a new one)
- `flow_value`: as before
- `cost_max`: edge cost is a random whole number from `[0, cost_max]`
- `capacity_max`:
    variable lower bound (minimum quantity) is in `[0, capacity_max]`
    upper bound (capacity) is in `[1, capacity_max]`, but ensuring `lower<=upper`


Current generator `Network generate_instance(Parameters p)` works as we discussed, but is otherwise quite bad.
Should be enough to test the solver.
Currently working on a better one.

## Compiling

- on Windows with MSYS2 or on Linux:

```
g++ -o test_serialization test_serialization.cpp generator.cpp network.cpp --std=c++17 -static-libgcc -static-libstdc++ -Wall
```