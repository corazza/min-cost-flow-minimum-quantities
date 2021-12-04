// #include <stdlib.h>
// #include <time.h>

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

#include "generator.hpp"
#include "mcnfmq.hpp"

int main() {
    // std::srand(time(NULL));
    ParametersUniform p;
    Mcnfmq instance = generate_uniform_instance(p);
    // json j = instance;
    // std::ofstream o("output.json");
    // o << std::setw(4) << j << std::endl;
    return 0;
}
