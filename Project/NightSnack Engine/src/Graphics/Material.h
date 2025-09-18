#pragma once

#include <glm/glm.hpp>
#include "Shader.h"

class Material {

public: 

	Material(Shader* shader, const glm::vec3& baseColor);

	Shader* GetShader() const;

	void ApplyUniforms() const;

private:

	Shader* m_shader = nullptr;
	glm::vec3 m_baseColor;

};