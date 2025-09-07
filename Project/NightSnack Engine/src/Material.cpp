#include "Material.h"

// Construct a Material with a shader program and a base color ------------------------------------
// ------------------------------------------------------------------------------------------------
Material::Material(Shader* shader, const glm::vec3& baseColor) :
	shader(shader), baseColor(baseColor) {
}

// Get the shader of this particular Material object ----------------------------------------------
// ------------------------------------------------------------------------------------------------
Shader* Material::GetShader() const {
	return shader;
}

// Pass the uniform variables to the shader programs-----------------------------------------------
// ------------------------------------------------------------------------------------------------
void Material::ApplyUniforms() const {
	shader->SetVec3("uBaseColor", baseColor);
}