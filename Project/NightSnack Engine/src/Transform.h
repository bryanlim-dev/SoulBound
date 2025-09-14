#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform {

	glm::vec2 position{ 0.f,0.f };
	float rotation{ 0.f }; // in radians
	glm::vec2 scale{ 1.f, 1.f };

	glm::mat4 GetModelToWorldMatrix() {
		glm::mat4 model(1.f);
		model = glm::translate(model, glm::vec3(position, 0.f));
		model = glm::rotate(model, rotation, glm::vec3(0.f, 0.f, 1.f));
		model = glm::scale(model, glm::vec3(scale, 1.f));
		return model;
	}

};