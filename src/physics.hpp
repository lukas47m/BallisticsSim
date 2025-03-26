#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include "components.hpp"

class Physics {
    public:

        static glm::dvec3 GRAVITY;
        static double AIR_DENSITY;

        Physics(){}
        virtual ~Physics(){}

        static void update(entt::registry& registry, double deltaTime){
            auto view = registry.view<Position, Velocity, Mass>();

            view.each([deltaTime](auto entity, auto& position, auto& velocity, auto& mass){

                glm::dvec3 vel = velocity.velocity + GRAVITY * deltaTime;

                double speed = glm::length(velocity.velocity);
                if(speed > 0.0){
                    double F_resistance = 0.5 * AIR_DENSITY * speed * speed * mass.air_resistance;
                    glm::dvec3 a_resistance = -glm::normalize(velocity.velocity) * F_resistance / mass.mass;
                    vel += a_resistance * deltaTime;
                }
                
                position.previous_position = position.position;
                position.position += (vel + velocity.velocity) * 0.5 * deltaTime;
                velocity.velocity = vel;

            });
        }

};

glm::dvec3 Physics::GRAVITY = glm::dvec3(0.0, -9.81, 0.0);
double Physics::AIR_DENSITY = 1.225;