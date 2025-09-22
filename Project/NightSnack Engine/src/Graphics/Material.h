#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

class Material {

public: 

	// Material with only a base color
	Material(Shader* shader, const glm::vec3& baseColor = glm::vec3(1.f));

	// Material with a texture
	Material(Shader* shader, Texture* texture, const glm::vec3& baseColor = glm::vec3(1.f));

	// Shader associated with this material
	Shader* GetShader() const;

	// Texture associated with this material
	Texture* GetTexture() const;

	// Pass uniform variables to the shader program
	void ApplyUniforms(bool isWireFrameMode) const;

private:

	Shader* m_shader = nullptr;
	Texture* m_texture = nullptr;
	glm::vec3 m_baseColor{ 1.f };
	bool m_EnableAlphaBlending{ false };

};