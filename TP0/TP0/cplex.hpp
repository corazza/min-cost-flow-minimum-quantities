#include "network.hpp"
#include <ilcplex/ilocplex.h>

double rjesenje_cplex(Network* graf, int flow_value, Flow* flow, std::set<edge_key>* variable_bounds);