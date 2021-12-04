#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "json.hpp"
using json = nlohmann::json;

#include "generator.hpp"
#include "network.hpp"

int main() {
    std::srand(time(NULL));
    Parameters p(100, 30, 15, 15);
    Network network = generate_instance(p);
    std::cout << "generated random network" << std::endl;

    json j = network;
    std::ofstream o("output.json");
    o << std::setw(4) << j << std::endl;
    o.close();
    std::cout << "serialized network to output.json" << std::endl;

    std::ifstream i("output.json");
    json j2;
    i >> j2;
    i.close();

    Network instance2 = j2.get<Network>();
    std::cout << "deserialized network from output.json" << std::endl;
    return 0;
}
