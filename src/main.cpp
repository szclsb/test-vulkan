#include "ove_first_app.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
    ove::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}