#pragma once

#include "mesh.hpp"


class Sphere : public Mesh {
    public:

    void create(size_t n){

        vertices = std::vector<Vertex>((n+1)*(n+1));
        indices.clear();

        const float pi =  glm::pi<float>();

    for (int i = 0; i <= n; ++i) {
        float phi = pi * i / n;
        float y = glm::cos(phi);
        float r = glm::sin(phi);

        for (int j = 0; j <= n; ++j) {
            float theta = 2.0f * pi * j / n;
            float x = r * glm::cos(theta);
            float z = r * glm::sin(theta);

            Vertex* vertex = &vertices[i*(n+1)+j];
            vertex->position.x = x/2;
            vertex->position.y = y/2;
            vertex->position.z = z/2;
            vertex->normal = vertex->position;

        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int first = (i * (n + 1)) + j;
            int second = first + n + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

        init();
    }
};