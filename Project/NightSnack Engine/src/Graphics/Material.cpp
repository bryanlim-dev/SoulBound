#include "Material.h"

// Construct a Material with a shader program and a base color ------------------------------------
// ------------------------------------------------------------------------------------------------
Material::Material(Shader* shader, const glm::vec3& baseColor) :
	m_shader(shader), m_texture(nullptr), m_baseColor(baseColor), m_EnableAlphaBlending(false) {
	// Default base color overlay is white unless specified
}

// Construct a Material with a shader program, a texture and a base color ------------------------------------
// ------------------------------------------------------------------------------------------------
Material::Material(Shader* shader, Texture* texture, const glm::vec3& baseColor) :
	m_shader(shader), m_texture(texture), m_baseColor(baseColor) {
	// Default base color overlay is white unless specified

	// Only if the texture is a png format too
	m_EnableAlphaBlending = m_texture && texture->HasAlpha();
	//m_EnableAlphaBlending = false;
	//if (textureName.find(".png") != std::string::npos)
	//	m_EnableAlphaBlending = true;
}

// Get the shader of this particular Material object ----------------------------------------------
// ------------------------------------------------------------------------------------------------
Shader* Material::GetShader() const {
	return m_shader;
}

// Get the texture of this particular Material object ---------------------------------------------
// ------------------------------------------------------------------------------------------------
Texture* Material::GetTexture() const {
	return m_texture;
}

//std::string Material::GetShaderName() const {
//	return m_shaderName;
//}
//
//std::string Material::GetTextureName() const {
//	return m_textureName;
//}

// Pass the uniform variables to the shader programs-----------------------------------------------
// ------------------------------------------------------------------------------------------------
void Material::ApplyUniforms(bool isWireFrameMode) const {
	if (!m_shader)
		return;

	m_shader->SetVec3("uBaseColor", m_baseColor);

	if (m_texture && !isWireFrameMode) { // This material has a texture, and is NOT wireframe mode
		m_shader->SetInt("uHasTexture", 1); // True
		
		glBindTextureUnit(0, m_texture->GetID()); // bind to texture unit 0
		m_shader->SetInt("uTex2d", 0); // tell shader to use unit 0

	} else { // This material only has color
		m_shader->SetInt("uHasTexture", 0); // False
	}

	// Enable alpha blending for png textures
	if (m_EnableAlphaBlending) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}

	
}