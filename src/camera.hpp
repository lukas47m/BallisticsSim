#pragma once

#include "transform.hpp"
class Camera
{
public:

	glm::mat4 getProjectionView() {
		return projectionMatrix * viewMatrix;
	}

	void setOrthographicProjection(
		float left, float right, float top, float bottom, float near, float far) {
		projectionMatrix = glm::mat4{ 1.0f };
		projectionMatrix[0][0] = 2.f / (right - left);
		projectionMatrix[1][1] = 2.f / (bottom - top);
		projectionMatrix[2][2] = 1.f / (far - near);
		projectionMatrix[3][0] = -(right + left) / (right - left);
		projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		projectionMatrix[3][2] = -near / (far - near);
	}

	void setAspect(float aspect) {
		this->aspect = aspect;
		const float tanHalfFovy = tan(fovy / 2.f);
		projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
	}
	void setFovy(float fovy) {
		this->fovy = fovy;
		const float tanHalfFovy = tan(fovy / 2.f);
		projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		projectionMatrix[1][1] = 1.f / (tanHalfFovy);
	}

	void setViewDirection( glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }) {
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };
		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, transform.position);
		viewMatrix[3][1] = -glm::dot(v, transform.position);
		viewMatrix[3][2] = -glm::dot(w, transform.position);
	}

	void setViewTarget(glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }) {
		setViewDirection( target - transform.position, up);
	}

	void setViewYXZ() {
		const float c3 = glm::cos(transform.rotation.z);
		const float s3 = glm::sin(transform.rotation.z);
		const float c2 = glm::cos(transform.rotation.x);
		const float s2 = glm::sin(transform.rotation.x);
		const float c1 = glm::cos(transform.rotation.y);
		const float s1 = glm::sin(transform.rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		viewMatrix = glm::mat4{ 1.f };
		viewMatrix[0][0] = u.x;
		viewMatrix[1][0] = u.y;
		viewMatrix[2][0] = u.z;
		viewMatrix[0][1] = v.x;
		viewMatrix[1][1] = v.y;
		viewMatrix[2][1] = v.z;
		viewMatrix[0][2] = w.x;
		viewMatrix[1][2] = w.y;
		viewMatrix[2][2] = w.z;
		viewMatrix[3][0] = -glm::dot(u, transform.position);
		viewMatrix[3][1] = -glm::dot(v, transform.position);
		viewMatrix[3][2] = -glm::dot(w, transform.position);
	}

	void setPerspectiveProjection(float fovy, float aspect, float near, float far) {
		this->fovy = fovy;
		this->aspect = aspect;
		const float tanHalfFovy = tan(fovy / 2.f);
		projectionMatrix = glm::mat4{ 0.0f };
		projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		projectionMatrix[2][2] = far / (far - near);
		projectionMatrix[2][3] = 1.f;
		projectionMatrix[3][2] = -(far * near) / (far - near);
	}

	glm::mat4 getProjection() { return projectionMatrix; }
	glm::mat4 getView() { return viewMatrix; }

	TransformComponent transform;

	float fovy;
private:
	glm::mat4 projectionMatrix{ 1.f };
	glm::mat4 viewMatrix{ 1.f };
	float aspect;
};
