#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

class Material {

public: 

	Material() {}
	// Material with only a base color
	Material(Shader* shader, const glm::vec3& baseColor = glm::vec3(1.f));

	// Material with a texture
	Material(Shader* shader, Texture* texture, const glm::vec3& baseColor = glm::vec3(1.f));

	// Shader associated with this material
	Shader* GetShader() const;

	// Texture associated with this material
	Texture* GetTexture() const;

	//std::string GetShaderName() const;
	//std::string GetTextureName() const;

	// Pass uniform variables to the shader program
	void ApplyUniforms(bool isWireFrameMode) const;

private:

	//std::string m_shaderName;
	//std::string m_textureName;
	//Shader m_shader;
	//Texture m_texture;
	Shader* m_shader = nullptr;
	Texture* m_texture = nullptr;
	glm::vec3 m_baseColor{ 1.f };
	bool m_EnableAlphaBlending{ false };

};