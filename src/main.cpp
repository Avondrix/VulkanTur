#include <iostream>
#include "TriangleApp.hpp"

int main() {
    TriangleApp app{};
    try {
        app.run();
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
