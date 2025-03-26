cmake_minimum_required(VERSION 3.14)
project(SimulationTest VERSION 0.1.0 LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Download and configure GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)

# Download and configure EnTT
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.12.2  # Using a specific version tag
)

FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.0  # Using a specific version tag
)

# Make the dependencies available
FetchContent_MakeAvailable(glm entt catch2)

# Create the executable
add_executable(${PROJECT_NAME} simulation_test.cpp)

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm EnTT::EnTT Catch2::Catch2WithMain) 