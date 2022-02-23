#include "network.hpp"
#include <ilcplex/ilocplex.h>
//parameters: network which a solution is being looked for in, value of the flow, flow into which the 
//solution is written in, set which variable_bound edges are written into
double rjesenje_cplex(Network* graf, int flow_value, Flow* flow);