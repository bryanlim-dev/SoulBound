#include "Renderer.h"

// Add a renderable object to the list of objects -------------------------------------------------
// Typically called when spawning a new entity ----------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Renderer::AddRenderable(const Renderable& renderable) {
	objects.emplace_back(renderable);
}

// Remove all currently stored objects in the list ------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Renderer::Clear() {
	objects.clear();
}

// Draw all renderable objects in the list --------------------------------------------------------
// Handles shader binding, uniform updates, wireframe toggling, and mesh drawing ------------------
// ------------------------------------------------------------------------------------------------
void Renderer::DrawAll(){

	Shader* currentShader = nullptr;

	for (auto& obj : objects) {
		if (!obj.mesh || !obj.material)
			continue;

		Shader* shader = obj.material->GetShader();

		// Only bind new shader if different from the last
		if (shader != currentShader) {
			shader->Use();
			currentShader = shader;
		}

		obj.material->ApplyUniforms(); // baseColor, textures etc

		if (obj.wireframe) // Change the polygon mode to wireframe
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		obj.mesh->Draw();

		if (obj.wireframe) // Reset the polygon mode back to the default
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

}