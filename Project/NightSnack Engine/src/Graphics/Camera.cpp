#include "Camera.h"

//Camera::Camera(float width, float height) : 
//	width(width), height(height) { }

glm::mat4 Camera::GetViewMatrix() {
	return glm::translate(glm::mat4(1.f), glm::vec3(-position, 0.f));
}

glm::mat4 Camera::GetProjectionMatrix() {
	float halfHeight = height * 0.5f;
	float halfWidth = width * 0.5f;
	return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.f, 1.f);
}

