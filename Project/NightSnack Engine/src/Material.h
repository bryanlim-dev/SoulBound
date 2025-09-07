#pragma once

#include <glm/glm.hpp>
#include "shader.h"

class Material {

public: 

	Material(Shader* shader, const glm::vec3& baseColor);

	Shader* GetShader() const;

	void ApplyUniforms() const;

private:

	Shader* shader = nullptr;
	glm::vec3 baseColor;

};