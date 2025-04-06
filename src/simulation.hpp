#pragma once

#include "components.hpp"
#include "physics.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <limits>
#include <functional>

class Simulation {
    private:
    static double HIT_TRASHOLD;
    static double MAX_SIMULATION_TIME;
    static double AIR_RESISTANCE;
    static uint32_t MAX_TRIES;

    public:
    static glm::dvec3 UP_VECTOR;
        enum ShotResultEnum{
            HIT,
            TOO_HIGH,
            TOO_LOW,
            NO_TIME,
            NO_IN_RANGE
        };

        struct ShotResult{
            ShotResultEnum result;
            double distance;
            double time;

        };

        struct StrategyResult{
            ShotResult best_result;
            double best_angle;
            uint32_t tries;
        };

        Simulation(){}
        void init(const glm::dvec3& shooter_position, const glm::dvec3& target_position, double shoot_speed, double shoot_height, double delta_time){
            this->shooter_position = shooter_position;
            this->target_position = target_position;
            this->shoot_speed = shoot_speed;
            this->shoot_height = shoot_height;
            this->delta_time = delta_time;
        }

        Simulation(const glm::dvec3& shooter_position, const glm::dvec3& target_position, double shoot_speed, double shoot_height, double delta_time) :
         shooter_position(shooter_position), target_position(target_position), shoot_speed(shoot_speed), shoot_height(shoot_height), delta_time(delta_time) {}   
        
        virtual ~Simulation(){}

        StrategyResult find_angle_strategy(std::function<void(const ShotResult& result, const double& angle)> callback = nullptr,
                                            std::function<void(const Position& position, const double& time)> callback2 = nullptr){
            StrategyResult best_result = {{ShotResultEnum::NO_TIME, std::numeric_limits<double>::max(), 0.0}, 0.0, 0};

            glm::dvec3 direction = glm::normalize(target_position - shooter_position);
            double dotProduct = glm::dot(glm::normalize(direction), UP_VECTOR);
            double max_angle = glm::degrees(glm::acos(dotProduct));
            double min_angle = 0.0;

            double angle = min_angle;
            
            uint32_t tries = 0;
            while(tries < MAX_TRIES){
                tries++;
                ShotResult result = simulateShot(angle, callback2);
                
                if(callback){
                    callback(result, angle);
                }

                if(best_result.best_result.distance > result.distance){
                    best_result = {result, angle, tries};
                }

                if(result.result == ShotResultEnum::HIT){
                    return best_result;
                }

                if(result.result == ShotResultEnum::TOO_HIGH){
                    max_angle = angle;
                }

                if(result.result == ShotResultEnum::TOO_LOW){
                    min_angle = angle;
                    if(best_result.best_result.result == ShotResultEnum::TOO_LOW){
                        if(best_result.best_result.distance < result.distance){
                            if(best_result.best_angle < angle){
                                best_result.best_result.result = ShotResultEnum::NO_IN_RANGE;
                                return best_result;
                            }
                        }
                    }
                }

                if(max_angle - min_angle < 0.000000001){
                    return best_result;
                }

                angle = (min_angle + max_angle) / 2.0;
            }
            return best_result;
        }
        

        ShotResult simulateShot(double angle, std::function<void(const Position& position, const double& time)> callback = nullptr){
            if(delta_time <= 0.0){
                return {ShotResultEnum::NO_TIME, 0.0, 0.0};
            }
            double time = 0.0;

            entt::registry registry;

            glm::dvec3 direction = glm::normalize(target_position - shooter_position);
            glm::dvec3 right = glm::normalize(glm::cross(direction, UP_VECTOR));
            if(glm::isnan(right.x)){
                right = glm::dvec3(1, 0, 0);
            }
            glm::dmat4 rotation = glm::rotate(glm::dmat4(1.0), glm::radians(angle), right);
            glm::dvec3 new_direction = rotation * glm::dvec4(direction, 1.0);
            glm::dvec3 velocity = new_direction * shoot_speed;

            auto projectile = registry.create();

            registry.emplace<Position>(projectile, shooter_position);
            registry.emplace<Velocity>(projectile, velocity);
            registry.emplace<Mass>(projectile, Mass{shoot_height, AIR_RESISTANCE});

            double min_distance = glm::length(shooter_position - target_position);

            while(time < MAX_SIMULATION_TIME){
                Physics::update(registry, delta_time);
                time += delta_time;

                const Position& position = registry.get<Position>(projectile);

                if(callback){
                    callback(position, time);
                }

                glm::dvec3 AB = position.position - position.previous_position;
                glm::dvec3 AP = target_position - position.previous_position;

                double t = glm::dot(AP, AB) / glm::dot(AB, AB);
                glm::dvec3 nearest_point = position.previous_position + glm::clamp(t, 0.0, 1.0) * AB;

                double distance = glm::length(target_position - nearest_point);

                if(distance < HIT_TRASHOLD){
                    return {ShotResultEnum::HIT, distance, time};
                }
                //I assume that I want to hit the target as directly as possible, without considering a higher arc trajectory.
                if(t<1.0){
                
                  if(glm::dot(target_position - nearest_point, UP_VECTOR) < 0.0){
                        return {ShotResultEnum::TOO_HIGH, distance, time};
                    }
                    return {ShotResultEnum::TOO_LOW, distance, time};
                }

                min_distance = distance;
            }

            return {ShotResultEnum::NO_TIME, min_distance, time};
        }
        
    private:
        glm::dvec3 shooter_position;
        glm::dvec3 target_position;
        double shoot_speed;
        double shoot_height;
        double delta_time;

};

double Simulation::HIT_TRASHOLD = 0.0000001;
double Simulation::MAX_SIMULATION_TIME = 100.0;
double Simulation::AIR_RESISTANCE = 0.01;
glm::dvec3 Simulation::UP_VECTOR = -glm::normalize(Physics::GRAVITY);
uint32_t Simulation::MAX_TRIES = 1000;