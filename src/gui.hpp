#pragma once

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include "simulation.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "shader.hpp"

class GUI {
    public:
        GLFWwindow* window;
        Simulation simulation;
        int windowWidth = 1280;
        int windowHeight = 720;

        std::thread simulation_thread;

        std::vector<glm::vec3> trajectory;

        Simulation::StrategyResult lastResult;
        bool hasResult = false;

        Camera camera;
        Sphere* sphere = nullptr;
        Shader* shader = nullptr;
        GLuint modelTransformID;
        GLuint modelColorID;
        GLuint projectionID;

        struct SimulationParameters{
            TransformComponent shooter_position = {glm::vec3(0.0f, 0.0f, 0.0f)};
            TransformComponent target_position = {glm::vec3(100.0f, 0.0f, 0.0f)};
            float shoot_speed = 100.0f;
            float shoot_height = 1.0f;
            float delta_time = 0.01f;

            float angle_start = 0.0f;
        } simulation_parameters;

        struct CameraParameters{
            glm::vec3 position = glm::vec3(100.0f, 100.0f, 100.0f);
            glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        } camera_parameters;

        struct PhysicsParameters{
            glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
            float air_density = 1.225f;
        } physics_parameters;

        GUI(){
            // Initialize GLFW
            if (!glfwInit()) {
                std::cerr << "Failed to initialize GLFW" << std::endl;
                return;
            }
            
            // GL 3.3 + GLSL 330
            const char* glsl_version = "#version 330 core";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            
            #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            #endif
            
            // Create window with graphics context
            window = glfwCreateWindow(windowWidth, windowHeight, "Projectile Simulation", NULL, NULL);
            if (window == NULL) {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return;
            }
            glfwMakeContextCurrent(window);
            glfwSwapInterval(1); // Enable vsync
            
            // Initialize GLAD
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cerr << "Failed to initialize GLAD" << std::endl;
                return;
            }

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            
            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            
            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);

            sphere = new Sphere();
            sphere->create(10);



            shader = new Shader();
            ShaderModule* vertexShader = new ShaderModule("../src/shaders/vertex.vert", GL_VERTEX_SHADER);
            ShaderModule* fragmentShader = new ShaderModule("../src/shaders/fragment.frag", GL_FRAGMENT_SHADER);
            shader->init({vertexShader, fragmentShader});

            modelTransformID = glGetUniformLocation(shader->shaderId, "modelTransform");
            modelColorID = glGetUniformLocation(shader->shaderId, "modelColor");
            projectionID = glGetUniformLocation(shader->shaderId, "projMat");

            camera.setPerspectiveProjection(glm::radians(50.f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);
        }
        ~GUI(){
            delete sphere;
            delete shader;
            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            
            glfwDestroyWindow(window);
            glfwTerminate();
        }
        void run(){

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
        
                // Clear the frame
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


                shader->bind();

                glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(camera.getProjectionView()));
                
                // Render the simulation visualization using OpenGL
                renderScene();   
                
                // Render ImGui elements
                renderGUI();
                
                glfwSwapBuffers(window);
            }
        }

        void renderGUI(){
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render ImGui elements
            ImGui::SetNextWindowPos(ImVec2(windowWidth - 350, 10), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(340, 400), ImGuiCond_FirstUseEver);
            
            ImGui::Begin("Projectile Motion Simulator");
            
            // Input parameters
            ImGui::Text("Simulation Parameters:");
            ImGui::SliderFloat3("Shooter Position", glm::value_ptr(simulation_parameters.shooter_position.position), -100.0f, 100.0f);
            ImGui::SliderFloat3("Target Position", glm::value_ptr(simulation_parameters.target_position.position), -100.0f, 100.0f);
            ImGui::SliderFloat("Shoot Speed", &simulation_parameters.shoot_speed, 1.0f, 500.0f);
            ImGui::SliderFloat("Shoot Height", &simulation_parameters.shoot_height, 0.001f, 10.0f);
            ImGui::SliderFloat("Time Step", &simulation_parameters.delta_time, 0.0001f, 1.0f);
            
            // Run simulation button
            if (ImGui::Button("Find Angle")) {
                simulation_thread = std::thread([this](){
                    simulation.init(simulation_parameters.shooter_position.position, simulation_parameters.target_position.position, simulation_parameters.shoot_speed, simulation_parameters.shoot_height, simulation_parameters.delta_time);
                    lastResult = simulation.find_angle_strategy2();
                    hasResult = true;
                });
                simulation_thread.detach();
            }
            
            // Display results if available
            if (hasResult) {
                ImGui::Separator();
                ImGui::Text("Simulation Results:");
                ImGui::Text("Best Angle: %.2f deg", lastResult.best_angle);
                ImGui::Text("Tries: %d", lastResult.tries);
                ImGui::Text("Distance: %.2f m", lastResult.best_result.distance);
                ImGui::Text("Time of Flight: %.2f s", lastResult.best_result.time);
                ImGui::Text("Result: %s", lastResult.best_result.result == Simulation::ShotResultEnum::HIT ? "HIT" : "MISS");
            }

            ImGui::SliderFloat("Angle", &simulation_parameters.angle_start, 0.0f, 90.0f);
            if (ImGui::Button("Shoot")) {
                simulation_thread = std::thread([this](){
                    trajectory.clear();
                    simulation.init(simulation_parameters.shooter_position.position, simulation_parameters.target_position.position, simulation_parameters.shoot_speed, simulation_parameters.shoot_height, simulation_parameters.delta_time);
                    simulation.simulateShot(simulation_parameters.angle_start, [this](const Position& position, const double& time){
                        trajectory.push_back(glm::vec3((float)position.position.x, (float)position.position.y, (float)position.position.z));
                    });
                });
                simulation_thread.detach();
            }
            
            ImGui::End();

            ImGui::Begin("Camera Parameters");
            ImGui::SliderFloat3("Position", glm::value_ptr(camera_parameters.position), -100.0f, 100.0f);
            ImGui::SliderFloat3("Target", glm::value_ptr(camera_parameters.target), -100.0f, 100.0f);
            ImGui::End();

            camera.transform.position = camera_parameters.position;
            camera.setViewTarget(camera_parameters.target);

            ImGui::Begin("Physics Parameters");
            ImGui::SliderFloat3("Gravity", glm::value_ptr(physics_parameters.gravity), -10.0f, 10.0f);
            ImGui::SliderFloat("Air Density", &physics_parameters.air_density, 0.0f, 2.0f);
            ImGui::End();

            Physics::GRAVITY = physics_parameters.gravity;
            Physics::AIR_DENSITY = physics_parameters.air_density;
            Simulation::UP_VECTOR = -glm::normalize(Physics::GRAVITY);
            
            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
        } 
        void renderScene(){
            
            shader->bind();


            simulation_parameters.shooter_position.scale = glm::vec3(10.0f, 10.0f, 10.0f);
            glUniformMatrix4fv(modelTransformID, 1, GL_FALSE, glm::value_ptr(simulation_parameters.shooter_position.mat4()));
            glUniform3fv(modelColorID, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
            sphere->bind();
            sphere->draw();

            simulation_parameters.target_position.scale = glm::vec3(10.0f, 10.0f, 10.0f);
            glUniformMatrix4fv(modelTransformID, 1, GL_FALSE, glm::value_ptr(simulation_parameters.target_position.mat4()));
            glUniform3fv(modelColorID, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
            sphere->bind();
            sphere->draw();

            TransformComponent trasform;
            for (int i = 0; i < trajectory.size(); i++){
                trasform.position = trajectory[i];
                glUniformMatrix4fv(modelTransformID, 1, GL_FALSE, glm::value_ptr(trasform.mat4()));
                glUniform3fv(modelColorID, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
                sphere->bind();
                sphere->draw();
            }
        }
};
