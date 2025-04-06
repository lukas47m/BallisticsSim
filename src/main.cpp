#include <iostream>
#include "simulation.hpp"
#include "gui.hpp"


int main() {
    try {
        // Create and run the simulation GUI
        GUI gui;
        gui.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 