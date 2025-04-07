#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_approx.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <entt/entt.hpp>

#define private public
#include "../src/physics.hpp"
#include "../src/simulation.hpp"
#include "../src/components.hpp"

TEST_CASE("Physics Test", "[physics]") {

    //For an atmospheric pressure of 0, the position and velocity can be determined exactly.
    Physics::AIR_DENSITY = 0.0;

    //These values can be freely set or placed into an array to test as many cases as possible, especially some edge cases.
    Physics::GRAVITY = glm::dvec3(0.0, -10, 0.0);
    glm::dvec3 initial_position = glm::dvec3(0.0, 0.0, 10.0);
    glm::dvec3 initial_velocity = glm::dvec3(10.0, 1.0, 0.0);
    double step_time = 10.0;

    entt::registry registry;

    auto entity = registry.create();
    registry.emplace<Position>(entity, initial_position);
    registry.emplace<Velocity>(entity, initial_velocity);
    registry.emplace<Mass>(entity, 1.0);

    Physics::update(registry, step_time);

    SECTION("Position Test"){
        auto position = registry.get<Position>(entity);

        glm::dvec3 expected_position = initial_position + initial_velocity * step_time + 0.5 * Physics::GRAVITY * step_time * step_time;

        REQUIRE(position.position.x == Catch::Approx(expected_position.x));
        REQUIRE(position.position.y == Catch::Approx(expected_position.y));
        REQUIRE(position.position.z == Catch::Approx(expected_position.z));
    }

    SECTION("Velocity Test"){
        auto velocity = registry.get<Velocity>(entity);

        glm::dvec3 expected_velocity = initial_velocity + Physics::GRAVITY * step_time;

        REQUIRE(velocity.velocity.x == Catch::Approx(expected_velocity.x));
        REQUIRE(velocity.velocity.y == Catch::Approx(expected_velocity.y));
        REQUIRE(velocity.velocity.z == Catch::Approx(expected_velocity.z));
    }
    //...
}

TEST_CASE("Shoot Simulation Test", "[simulation]") {
    
    Physics::AIR_DENSITY = 0.0;

    Physics::GRAVITY = glm::dvec3(0.0, 0.0, 0.0);
    glm::dvec3 initial_position = glm::dvec3(0.0, 0.0, 10.0);
    glm::dvec3 target_position = glm::dvec3(10.0, 1.0, 0.0);
    double step_time = 0.01;

    Simulation::HIT_TRASHOLD = 0.0000001;
    Simulation::MAX_SIMULATION_TIME = 100.0;

    Simulation simulation(initial_position, target_position, 200.0, 10.0, step_time);


    SECTION("Hit"){
        auto result = simulation.simulateShot(0.0);
        REQUIRE(result.result == Simulation::ShotResultEnum::HIT);
    }

    SECTION("Too High"){
        auto result = simulation.simulateShot(1.0);
        REQUIRE(result.result == Simulation::ShotResultEnum::TOO_HIGH);
    }

    SECTION("Too Low"){
        auto result = simulation.simulateShot(-1.0);
        REQUIRE(result.result == Simulation::ShotResultEnum::TOO_LOW);
    }

    SECTION("No Time"){
        Simulation::MAX_SIMULATION_TIME = .000001;
        auto result = simulation.simulateShot(0.0);
        REQUIRE(result.result == Simulation::ShotResultEnum::NO_TIME);
    }
    // ...
}

TEST_CASE("Simulation Strategy 1 Test", "[simulation]") {
    
    Physics::AIR_DENSITY = 0.0;

    double step_time = 0.01;

    Simulation::HIT_TRASHOLD = 0.0000001;
    Simulation::MAX_SIMULATION_TIME = 100.0;


    SECTION("hit"){
        Physics::GRAVITY = glm::dvec3(0.0, -10.0, 0.0);
        glm::dvec3 initial_position = glm::dvec3(0.0, 0.0, 0.0);
        glm::dvec3 target_position = glm::dvec3(10.0, 0.0, 0.0);
        Simulation simulation(initial_position, target_position, 10.0, 10.0, step_time*0.01);
        auto result = simulation.find_angle_strategy();
        REQUIRE(result.best_result.result == Simulation::ShotResultEnum::HIT);
        REQUIRE(result.best_angle == Catch::Approx(45.0));
    }
    SECTION("hit up"){
        Physics::GRAVITY = glm::dvec3(0.0, -10.0, 0.0);
        glm::dvec3 initial_position = glm::dvec3(0.0, 0.0, 0.0);
        glm::dvec3 target_position = glm::dvec3(0.0, 10.0, 0.0);
        Simulation simulation(initial_position, target_position, 100.0, 10.0, step_time);
        auto result = simulation.find_angle_strategy();
        REQUIRE(result.best_result.result == Simulation::ShotResultEnum::HIT);
        REQUIRE(result.best_angle == Catch::Approx(0.0));
    }
    SECTION("hit down"){
        Physics::GRAVITY = glm::dvec3(0.0, -10.0, 0.0);
        glm::dvec3 initial_position = glm::dvec3(5.0, 50.0, 10.0);
        glm::dvec3 target_position = glm::dvec3(5.0, -100.0, 10.0);
        Simulation simulation(initial_position, target_position, 0.00000, 10.0, step_time);
        auto result = simulation.find_angle_strategy();
        REQUIRE(result.best_result.result == Simulation::ShotResultEnum::HIT);
        REQUIRE(result.best_angle == Catch::Approx(0.0));
    }
    SECTION("no hit"){
        Physics::GRAVITY = glm::dvec3(0.0, -10.0, 0.0);
        glm::dvec3 initial_position = glm::dvec3(0.0, 0.0, 0.0);
        glm::dvec3 target_position = glm::dvec3(0.0, 10.0, 0.0);
        Simulation simulation(initial_position, target_position, 0.1, 10.0, step_time);
        auto result = simulation.find_angle_strategy();
        REQUIRE(result.best_result.result != Simulation::ShotResultEnum::HIT);
    }
    // ...
}

TEST_CASE("Simulation Strategy 2 Test", "[simulation]") {
    
    double step_time = 0.01;

    Simulation::HIT_TRASHOLD = 0.0000001;
    Simulation::MAX_SIMULATION_TIME = 100.0;
    Physics::AIR_DENSITY = 1.0;


    SECTION("hit"){
        Physics::GRAVITY = glm::dvec3(0.0, -10.0, 0.0);
        glm::dvec3 initial_position = glm::dvec3(0.0, 0.0, 0.0);
        glm::dvec3 target_position = glm::dvec3(10.0, 10.0, 10.0);
        Simulation simulation(initial_position, target_position, 100.0, 10.0, step_time*0.01);
        auto result = simulation.find_angle_strategy();
        REQUIRE(result.best_result.result == Simulation::ShotResultEnum::HIT);
    }
    SECTION("no hit"){
        Physics::GRAVITY = glm::dvec3(0.0, -10.0, 0.0);
        glm::dvec3 initial_position = glm::dvec3(0.0, 0.0, 0.0);
        glm::dvec3 target_position = glm::dvec3(10.0, 0.0, 0.0);
        Simulation simulation(initial_position, target_position, 10.0, 10.0, step_time*0.01);
        auto result = simulation.find_angle_strategy();
        REQUIRE(result.best_result.result != Simulation::ShotResultEnum::HIT);
    }
    // ...
}