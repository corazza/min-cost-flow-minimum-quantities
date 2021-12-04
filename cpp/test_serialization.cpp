#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include "generator.hpp"
#include "mcnfmq.hpp"

int main() {
    std::srand(time(NULL));
    ParametersUniform p(15, 30, 15, 15);
    Mcnfmq instance = generate_uniform_instance(p);

    json j = instance;
    std::ofstream o("output.json");
    o << std::setw(4) << j << std::endl;
    o.close();

    std::ifstream i("output.json");
    json j2;
    i >> j2;
    i.close();

    Mcnfmq instance2 = j2.get<Mcnfmq>();

    return 0;
}
