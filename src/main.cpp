#include <iostream>
#include "simulation.hpp"


int main() {
    Simulation simulation(glm::dvec3(0.0, 0.0, 0.0), glm::dvec3(10.0, 10.0, 0.0), 200.0, 10.0, 0.00001);
    auto result = simulation.find_angle_strategy([](const Simulation::ShotResult& result, const double& angle){
                std::cout << "--------------------------------" << std::endl;
                std::cout << "Angle: " << angle << " Result: " << result.result << " Distance: " << result.distance << " Time: " << result.time << std::endl;
    });
    std::cout << std::endl<< "Best angle: " << result.best_angle << " Tries: " << result.tries << " Result: " << result.best_result.result << " Distance: " << result.best_result.distance << " Time: " << result.best_result.time << std::endl;
    return 0;
} 