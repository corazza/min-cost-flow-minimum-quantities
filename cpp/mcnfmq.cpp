#include "mcnfmq.hpp"

void to_json(json& j, const Mcnfmq& instance) {
    j = json{
        {"n_nodes", instance.n_nodes},
        {"source", instance.source},
        {"sink", instance.sink},
        {"flow_value", instance.flow_value},
        {"costs", instance.costs},
        {"capacities", instance.capacities},
        {"minimum_quantities", instance.minimum_quantities},
    };
}

void from_json(const json& j, Mcnfmq& instance) {
    j.at("n_nodes").get_to(instance.n_nodes);
    j.at("source").get_to(instance.source);
    j.at("sink").get_to(instance.sink);
    j.at("flow_value").get_to(instance.flow_value);
}
