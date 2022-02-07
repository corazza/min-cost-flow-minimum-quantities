// Flow mutate(Network &network, Flow &original) {
//     int old_value = original.flow_value();
//     Flow flow = original.make_copy();  // TODO assert test

//     std::vector<edge_key> edge_keys;
//     for (auto a : flow.values) {
//         edge_keys.push_back(a.first);
//     }
//     int n_edges = edge_keys.size();
//     int removing_i = rand() % n_edges;
//     auto vertices = get_vertex_keys(edge_keys[removing_i]);
//     auto v_from = vertices.first;
//     auto v_to = vertices.second;
//     int removing_value = flow.values[edge_keys[removing_i]];
//     assert(removing_value > 0);
//     propagate(network, flow, v_to, -removing_value, true);
//     propagate(network, flow, v_from, -removing_value, false);
//     flow.remove_edge(v_from, v_to);
//     propagate(network, flow, network.source, removing_value, true);

//     int new_value = flow.flow_value();
//     assert(new_value == old_value);

//     return flow;
// }

// std::vector<Flow> decompose(const Flow &f, const FlowNetwork &network) {
//     Flow flow_copy = f.make_copy();
//     std::vector<Flow> flow_decomposition;

//     Flow tmp_flow(1, network.source, network.sink);

//     while (flow_copy.value) {
//         tmp_flow.empty_flow();

//         // find a unitary flow and subtract the flow from the copy

//         std::set<vertex_key>
//             visited;  // keeps a set of visited states, this should be redundant with the closed
//                       // nodes set, which structure has the quickest find implementation?
//         std::stack<std::vector<vertex_key>>
//             stack;  // keeps the nodes yet to be opened; a stack of maps might be a better choice
//         std::set<std::vector<vertex_key>>
//             closed_nodes;  // keeps all of the states and their parents

//         vertex_key parent = 0;
//         std::vector<vertex_key> node(2);
//         std::vector<vertex_key> tmp(2);
//         node[0] = node[1] = 0;
//         stack.push(node);
//         visited.insert(node[0]);
//         closed_nodes.insert(node);

//         do {  // find a valid unitary flow
//             tmp = stack.top();
//             parent = tmp[0];
//             if (visited.find(node[0]) == visited.end()) {
//                 for (auto it = flow_copy.outgoing[tmp[0]].begin();
//                      it != flow_copy.outgoing[tmp[0]].end(); ++it) {
//                     tmp[0] = *it;
//                     tmp[1] = parent;
//                     stack.push(tmp);
//                     visited.insert(tmp[0]);
//                     closed_nodes.insert(tmp);
//                 }
//             }
//             stack.pop();

//         } while (tmp[0] != network.n_nodes);

//         node = tmp;

//         do {  // make the path into a flow and subtract the flow from flow_copy
//             tmp_flow.add_edge(node[1], node[0], 1);
//             tmp_flow.subtract_from_edge(node[1], node[0], 1);

//             auto it = closed_nodes.begin();
//             while ((*it)[0] != node[1]) ++it;  // find the parent node in the closed_nodes set
//             node = *it;
//         } while (node[1] != 0);

//         flow_decomposition.push_back(tmp_flow.make_copy());

//         --flow_copy.value;
//     }

//     return flow_decomposition;
// }
