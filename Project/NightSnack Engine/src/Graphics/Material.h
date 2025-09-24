#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

class Material {
public:
    Material(Shader* shader, Texture* texture = nullptr)
        : m_shader(shader), m_texture(texture) {
    }

    Shader* GetShader() const { return m_shader; }
    Texture* GetTexture() const { return m_texture; }

    void SetBaseColor(float r, float g, float b, float a) {
        m_baseColor = glm::vec4{ r, g, b, a };
    }

    void ApplyUniforms(bool wireframe) {

        m_shader->Use();
        m_shader->SetVec3("uBaseColor", m_baseColor);

        if (m_texture) {
            m_texture->Bind(); // calls glBindTexture(GL_TEXTURE_2D, m_id)
            m_shader->SetInt("uTex2d", 0);
            m_shader->SetBool("uHasTexture", true);
        }
        m_shader->SetBool("uWireframe", wireframe);
    }

private:
    Shader* m_shader;
    Texture* m_texture;
    glm::vec4 m_baseColor{ 1.f, 1.f, 1.f, 1.f};
};

//class Material {
//
//public: 
//
//	Material() {}
//	// Material with only a base color
//	Material(Shader* shader, const glm::vec3& baseColor = glm::vec3(1.f));
//
//	// Material with a texture
//	Material(Shader* shader, Texture* texture, const glm::vec3& baseColor = glm::vec3(1.f));
//
//	// Shader associated with this material
//	Shader* GetShader() const;
//
//	// Texture associated with this material
//	Texture* GetTexture() const;
//
//	//std::string GetShaderName() const;
//	//std::string GetTextureName() const;
//
//	// Pass uniform variables to the shader program
//	void ApplyUniforms(bool isWireFrameMode) const;
//
//private:
//
//	//std::string m_shaderName;
//	//std::string m_textureName;
//	//Shader m_shader;
//	//Texture m_texture;
//	Shader* m_shader = nullptr;
//	Texture* m_texture = nullptr;
//	glm::vec3 m_baseColor{ 1.f };
//	bool m_EnableAlphaBlending{ false };
//
//};