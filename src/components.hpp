#pragma once

#include <glm/glm.hpp>

struct Position {
    glm::dvec3 position;
    glm::dvec3 previous_position;
};

struct Velocity {
    glm::dvec3 velocity;
};

struct Mass {
    double mass;
    double air_resistance; // area * dragg coefficient
};