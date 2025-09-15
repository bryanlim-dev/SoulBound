#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <iostream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include <glm/gtc/matrix_transform.hpp>

class GraphicsSystem : public ISystem {

public:
	// Default constructor to set up SystemSignature;
	GraphicsSystem() {
		// The Graphics System only cares about entities with these components
		// - Transform (position, rotation, scale)
		// - Renderable (mesh, material)
		SystemSignature.set(GetComponentTypeID<Transform>());
		SystemSignature.set(GetComponentTypeID<Renderable>());
	}

	// Find the Main Camera in the scene --------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------
	Entity FindCamera(Coordinator& Coord) {
		// Camera Signature (only have components IsCamera and Transform)
		Signature CameraSig;
		CameraSig.set(GetComponentTypeID<Camera>());
		CameraSig.set(GetComponentTypeID<Transform>());

		// Iterate through all Entities in the COORDINATOR instead
		// Since the GraphicsSystem will not have a Camera Entity
		for (Entity e = 0; e < Coord.GetEntityCount(); ++e) {
			if ((Coord.GetEntitySignature(e) & CameraSig) == CameraSig &&
				Coord.GetComponent<Camera>(e).isMainCam) { // This camera Entity is the main camera
				return e;
			}
		}
		std::cerr << "Error: No main camera found in scene!\n";
		return INVALID_ENTITY;
	}

	// Compute the projection matrix ------------------------------------------------------------------
	// Transforms camera coords to clip coords --------------------------------------------------------
	glm::mat4 GetCameraProjectionMatrix(float width, float height) {
		float halfWidth = width * 0.5f;
		float halfHeight = height * 0.5f;
		// Orthographic projection keeps objects the same size regardless of depth
		// Arguments in the order of (left, right, bottom, top, near, far)
		return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.f, 1.f);
	}

	// Compute the ModelToWorld matrix ----------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------
	glm::mat4 GetModelToWorldMatrix(Transform transform) const {
		glm::mat4 model(1.f);
		model = glm::translate(model, glm::vec3(transform.position, 0.f));
		model = glm::rotate(model, transform.rotation, glm::vec3(0.f, 0.f, 1.f)); // Rotate about z-axis
		model = glm::scale(model, glm::vec3(transform.scale, 1.f));
		return model;
	}

	// Core update loop -------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------
	void Update(double dt, Coordinator& Coord) override {
		(void)dt;

		Shader* currentShader = nullptr;

		// Get the main camera first, for matrix transformations
		Entity mainCam = FindCamera(Coord);
		if (mainCam == INVALID_ENTITY)
			return;

		// Get camera components
		auto& camTransform = Coord.GetComponent<Transform>(mainCam);
		auto& camInfo = Coord.GetComponent<Camera>(mainCam);
		
		// camera to clip coords
		glm::mat4 projMatrix = GetCameraProjectionMatrix(camInfo.width, camInfo.height);

		// Iterate through all Entities this system can access;
		for (auto const& e : Entities) {

			// First obtain references to components of interest;
			auto& transform = Coord.GetComponent<Transform>(e);
			auto& renderable = Coord.GetComponent<Renderable>(e);

			if (!renderable.mesh || !renderable.material)
				continue;

			// model to world coords
			glm::mat4 modelMatrix = GetModelToWorldMatrix(transform); 

			// Translate by negative camera position instead
			// world to camera coords
			glm::mat4 viewMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-camTransform.position, 0.f)); 

			glm::mat4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;

			Shader* shader = renderable.material->GetShader();

			if (shader) {
				// Only bind new shader if different from the last
				if (shader != currentShader) {
					shader->Use();
					currentShader = shader;
				}

				shader->SetMat4("uMVP", mvpMatrix);
			}

			renderable.material->ApplyUniforms(); // baseColor, textures etc

			if (renderable.isWireframe) // Change the polygon mode to wireframe
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			renderable.mesh->Draw();

			if (renderable.isWireframe) // Reset the polygon mode back to the default
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	Signature GetSignature() const override { return SystemSignature; }

private:
	Signature SystemSignature{};
};

// Declare and define more functions that will help you do stuff...

// Example:
// void SomeHelperFunction() {}
// ^ This function will then be called in either Update() or another function
//   that is called in Update();


#endif // GRAPHICS_HPP