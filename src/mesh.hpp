#pragma once


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <vector>

class Buffer
{
public:
	Buffer() {
		glGenBuffers(1, &ID);
	}
	template<class T>
	void init(std::vector<T>* vec, int t) {
		type = t;
		glBindBuffer(t, ID);
		glBufferData(t, vec->size() * sizeof(T), vec->data(), GL_STATIC_DRAW);
	}
	~Buffer() {
		glDeleteBuffers(1, &ID);
	};

	void bind() {

		glBindBuffer(type, ID);
	}
	void unBind() {

		glBindBuffer(type, 0);
	}
private:
	GLuint ID;
	int type;
};


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
};


class Mesh
{
public:
    
	GLuint ID;
	Buffer vertexBuffer;
	Buffer indexBuffer;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Mesh()  { 
		glGenVertexArrays(1, &ID);
	}
	~Mesh() {
		glDeleteVertexArrays(1, &ID);
	}

	void init() {
		bind();
		vertexBuffer.init(&vertices, GL_ARRAY_BUFFER);
		indexBuffer.init(&indices, GL_ELEMENT_ARRAY_BUFFER);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

		unBind();
		vertexBuffer.unBind();
		indexBuffer.unBind();
	}

	void bind() {
		glBindVertexArray(ID);
	}
	void unBind() {
		glBindVertexArray(0);
	}
	void draw () {
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
};


