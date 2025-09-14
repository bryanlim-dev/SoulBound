#include "Renderer.h"

// Add a renderable object to the list of objects -------------------------------------------------
// Typically called when spawning a new entity ----------------------------------------------------
// ------------------------------------------------------------------------------------------------
void Renderer::AddGameObject(GameObject* obj) {
	//objects.emplace_back(renderable);
	objects.push_back(obj);
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

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 proj = camera.GetProjectionMatrix();

	for (auto* obj : objects) {
		if (!obj->renderable.mesh || !obj->renderable.material)
			continue;

		glm::mat4 model = obj->transform.GetModelToWorldMatrix();
		glm::mat4 mvp = proj * view * model;

		Shader* shader = obj->renderable.material->GetShader();

		if (shader) {
			// Only bind new shader if different from the last
			if (shader != currentShader) {
				shader->Use();
				currentShader = shader;
			}

			shader->SetMat4("uMVP", mvp);
		}

		obj->renderable.material->ApplyUniforms(); // baseColor, textures etc

		if (obj->renderable.wireframe) // Change the polygon mode to wireframe
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		obj->renderable.mesh->Draw();

		if (obj->renderable.wireframe) // Reset the polygon mode back to the default
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

}