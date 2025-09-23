#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <iostream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Systems/Input.h"
#include "ResourceManager/ResourceManager.h"

class GraphicsSystem : public ISystem {

public:
	// Default constructor to set up SystemSignature;
	GraphicsSystem() {
		// The Graphics System only cares about entities with these components
		// - Position, Rotation, Scale
		// - Renderable (mesh, material)
		m_systemSignature.set(GetComponentTypeID<Position>());
		m_systemSignature.set(GetComponentTypeID<Rotation>());
		m_systemSignature.set(GetComponentTypeID<Scale>());
		m_systemSignature.set(GetComponentTypeID<Renderable>());
	}

	// Create the different meshes and materials first ------------------------------------------------
	// ------------------------------------------------------------------------------------------------
	void Init() {
		// Load shader
		Shader shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
		/*m_shaders.emplace("defaultShader", Shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag"));
		auto shaderIt = m_shaders.find("defaultShader");
		if (shaderIt == m_shaders.end()) {
			std::cerr << "Failed to load default shader\n";
			return;
		}
		Shader* defaultShader = &shaderIt->second;*/

		//m_shaders.emplace("defaultShader", Shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag"));
		//Shader* defaultShader = &m_shaders.at("defaultShader");

		// Load textures
		/*std::size_t bananaKey = 0;
		std::size_t targetKey = 0;
		ResourceManager* rm = ResourceManager::GetInstance();
		rm->LoadImage("assets/Banana.jpg", bananaKey);
		rm->LoadImage("assets/image_UI_target.png", targetKey);

		ResourceImage* imgBanana = rm->GetImageResource(bananaKey);
		ResourceImage* imgTarget = rm->GetImageResource(targetKey);

		unsigned char* pixelsBanana = rm->GetImage(bananaKey);
		unsigned char* pixelsTarget = rm->GetImage(targetKey);

		m_textures.emplace("bananaTexture", Texture(pixelsBanana, imgBanana->Width(), imgBanana->Height(), imgBanana->Channels()));
		m_textures.emplace("targetTexture", Texture(pixelsTarget, imgTarget->Width(), imgTarget->Height(), imgTarget->Channels()));
		
		auto bananaTextureIt = m_textures.find("bananaTexture");
		auto targetTextureIt = m_textures.find("targetTexture");*/

		// Default Meshes
		//m_meshes.emplace("pointMesh", Mesh::CreatePoint());
		//m_meshes.emplace("lineMesh", Mesh::CreateLine({0.f, 0.f}, {100.f, 0.f}));
		//m_meshes.emplace("squareMesh", Mesh::CreateSquare());
		//m_meshes.emplace("circleMesh", Mesh::CreateCircle());
		m_meshes["circleMesh"] = Mesh::CreateCircle();


		/*if (bananaTextureIt != m_textures.end()) {
			m_materials.emplace("bananaMaterial", Material(defaultShader, &bananaTextureIt->second));
		}*/

		//m_materials.emplace("whiteMaterial", Material(defaultShader, glm::vec3(1.f)));
		//m_materials.emplace("whiteMaterial", Material(&shader, glm::vec3(1.f)));
		m_materials["whiteMaterial"] = Material(&shader, glm::vec3(1.f));

	}

	// Find the Main Camera in the scene --------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------
	Entity FindCamera(Coordinator& Coord) {
		// Camera Signature (only have components Camera and Tranform)
		Signature cameraSig;
		cameraSig.set(GetComponentTypeID<Camera>());
		cameraSig.set(GetComponentTypeID<Position>());
		cameraSig.set(GetComponentTypeID<Rotation>());
		cameraSig.set(GetComponentTypeID<Scale>());

		// Iterate through all living Entities in the COORDINATOR instead
		// Since the GraphicsSystem will not have a Camera Entity
		for (const Entity e : Coord.GetLivingEntities()) {
			if ((Coord.GetEntitySignature(e) & cameraSig) == cameraSig &&
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
	glm::mat4 GetModelToWorldMatrix(Position position, Rotation rotation, Scale scale) const {
		glm::mat4 model(1.f);
		model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));
		model = glm::rotate(model, glm::radians(rotation.degrees), glm::vec3(0.f, 0.f, 1.f)); // Rotate about z-axis
		model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.f));
		return model;
	}

	// Core update loop -------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------
	void Update(float dt, Coordinator& coord) override {
		(void)dt;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Test key pressed
		if (coord.GetSystem<InputSystem>().get()->IsKeyPressed(InputSystem::W))
			std::cout << "w key pressed!!!\n";

		Shader* currentShader = nullptr;

		// Get the main camera first, for matrix transformations
		Entity mainCam = FindCamera(coord);
		if (mainCam == INVALID_ENTITY)
			return;

		// Get camera components
		auto& camPosition = coord.GetComponent<Position>(mainCam);
		auto& camInfo = coord.GetComponent<Camera>(mainCam);
		
		// camera to clip coords
		glm::mat4 projMatrix = GetCameraProjectionMatrix(camInfo.width, camInfo.height);

		// Iterate through all Entities this system can access;
		for (auto const& e : m_entities) {

			// First obtain references to components of interest;
			auto& position = coord.GetComponent<Position>(e);
			auto& rotation = coord.GetComponent<Rotation>(e);
			auto& scale = coord.GetComponent<Scale>(e);
			auto& renderable = coord.GetComponent<Renderable>(e);

			Mesh* mesh = renderable.mesh;
			Material* material = renderable.material;

			/*if (!mesh) std::cout << "Missing mesh: " << renderable.meshName << "\n";
			if (!material) std::cout << "Missing material: " << renderable.materialName << "\n";*/
			if (!mesh || !material)
				continue;

			// model to world coords
			glm::mat4 modelMatrix = GetModelToWorldMatrix(position, rotation, scale); 

			// Translate by negative camera position instead
			// world to camera coords
			glm::mat4 viewMatrix = glm::translate(glm::mat4(1.f), glm::vec3(-camPosition.x, -camPosition.y, 0.f));

			glm::mat4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;

			Shader* shader = renderable.material->GetShader();

			if (!shader) {
				std::cout << "Missing shader\n";
				continue;
			}

			Texture* texture = renderable.material->GetTexture();


			if (shader) {
				// Only bind new shader if different from the last
				if (shader != currentShader) {
					shader->Use();
					currentShader = shader;
				}

				shader->SetMat4("uMVP", mvpMatrix);
			}

			material->ApplyUniforms(renderable.isWireframe); // baseColor, textures etc

			if (renderable.isWireframe) // Change the polygon mode to wireframe
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			/*std::cout << "Drawing: Mesh=" << renderable.meshName
				<< " Mat=" << renderable.materialName
				<< " ShaderID=" << (shader ? shader->GetId() : 0)
				<< " TexID=" << (texture ? texture->GetID() : 0)
				<< "\n";*/

			mesh->Draw();

			if (renderable.isWireframe) // Reset the polygon mode back to the default
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	Mesh* GetMesh(const std::string& name) {
		auto it = m_meshes.find(name);
		return (it != m_meshes.end()) ? &it->second : nullptr;
	}

	Material* GetMaterial(const std::string& name) {
		auto it = m_materials.find(name);
		return (it != m_materials.end()) ? &it->second : nullptr;
	}

	Shader* GetShader(const std::string& name) {
		auto it = m_shaders.find(name);
		return (it != m_shaders.end()) ? &it->second : nullptr;
	}

	Texture* GetTexture(const std::string& name) {
		auto it = m_textures.find(name);
		return (it != m_textures.end()) ? &it->second : nullptr;
	}

	Signature GetSignature() const override { return m_systemSignature; }

	std::string GetName() const { return "GraphicsSystem"; }

private:
	Signature m_systemSignature{};

	std::unordered_map<std::string, Mesh> m_meshes;
	std::unordered_map<std::string, Material> m_materials;
	std::unordered_map<std::string, Shader> m_shaders;
	std::unordered_map<std::string, Texture> m_textures;
};

// Declare and define more functions that will help you do stuff...

// Example:
// void SomeHelperFunction() {}
// ^ This function will then be called in either Update() or another function
//   that is called in Update();


#endif // GRAPHICS_HPP