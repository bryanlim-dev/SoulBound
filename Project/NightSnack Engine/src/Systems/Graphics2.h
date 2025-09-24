#pragma once

#include <iostream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Systems/Input.h"

#include <glm/gtc/matrix_transform.hpp>


class GraphicsSystem2 : public ISystem {

public:
	// Default constructor to set up SystemSignature;
	GraphicsSystem2() {
		m_systemSignature.set(GetComponentTypeID<Position>());
		m_systemSignature.set(GetComponentTypeID<Rotation>());
		m_systemSignature.set(GetComponentTypeID<Scale>());
		m_systemSignature.set(GetComponentTypeID<Renderable2>());
	}

	// Main update called by core;
	void Update(float dt, Coordinator& coord) override;

	// Helper functions ------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	void Initialize();
	// Resource Creation;
	void AddShader(std::string const& name, std::unique_ptr<Shader> shader);
	void AddTexture(std::string const& name, std::unique_ptr<Texture> tex);
	void AddMesh(std::string const& name, std::unique_ptr<Mesh> mesh);
	void AddMaterial(std::string const& name, std::unique_ptr<Material> material);

	// Resource Lookup -----------------------------------------------
	Shader*		GetShader(std::string const& n)   const;	// Using raw pointers as the resource belongs to the System...
	Texture*	GetTexture(std::string const& n)  const;
	Mesh*		GetMesh(std::string const& n)     const;
	Material*	GetMaterial(std::string const& n) const;

	// Required by ISystem inheritance;
	Signature GetSignature() const override { return m_systemSignature; }
	std::string GetName() const { return "GraphicsSystem"; }

private:
	// Entity → cached resolved pointers;
	struct CachedRenderData {
		Mesh* mesh{ nullptr };
		Material* material{ nullptr };
		bool valid{ false };
	};
	// Associates each entity with its “ready-to-draw” data;
	std::unordered_map<Entity, CachedRenderData> m_renderCache;

	// Resource stores (owned by system);
	std::unordered_map<std::string, std::unique_ptr<Shader>>   m_shaders;
	std::unordered_map<std::string, std::unique_ptr<Texture>>  m_textures;
	std::unordered_map<std::string, std::unique_ptr<Mesh>>     m_meshes;
	std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

	// Helper functions;
	void ResolveRenderable(Entity e, Renderable2 const& r);
	Entity FindCamera(Coordinator& Coord);

	Signature m_systemSignature{};

};

// Compute the projection matrix ------------------------------------------------------------------
// Transforms camera coords to clip coords --------------------------------------------------------
static glm::mat4 GetCameraProjectionMatrix(float width, float height) {
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	// Orthographic projection keeps objects the same size regardless of depth
	// Arguments in the order of (left, right, bottom, top, near, far)
	return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.f, 1.f);
}

// Compute the ModelToWorld matrix ----------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
static glm::mat4 GetModelToWorldMatrix(Position position, Rotation rotation, Scale scale) {
	glm::mat4 model(1.f);
	model = glm::translate(model, glm::vec3(position.x, position.y, 0.f));
	model = glm::rotate(model, glm::radians(rotation.degrees), glm::vec3(0.f, 0.f, 1.f)); // Rotate about z-axis
	model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.f));
	return model;
}

inline void GraphicsSystem2::Update(float dt, Coordinator& coord) {
	(void)dt; // dt is unused for now...

	// Camera stuff...
	Entity mainCam = FindCamera(coord);
	if (mainCam == INVALID_ENTITY) return;

	// Get camera components
	auto& camPos = coord.GetComponent<Position>(mainCam);
	auto& camInfo = coord.GetComponent<Camera>(mainCam);

	glm::mat4 proj = GetCameraProjectionMatrix(camInfo.width, camInfo.height);
	glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camPos.x, -camPos.y, 0.f));

	for (Entity e : m_entities) {
		// Obtain entity components;
		auto& ren = coord.GetComponent<Renderable2>(e);
		auto& pos = coord.GetComponent<Position>(e);
		auto& rot = coord.GetComponent<Rotation>(e);
		auto& scl = coord.GetComponent<Scale>(e);

		// @TODO: Event-Driven check whether e's renderable was changed...
		// if (renderable was changed) { ResolveRenderable(e, renderable);

		// Find entity's renderCache;
		auto it = m_renderCache.find(e);
		if (it == m_renderCache.end() || !it->second.valid) {
			ResolveRenderable(e, ren);
			it = m_renderCache.find(e);
		}

		CachedRenderData& data = it->second;
		if (!data.valid) continue;


		glm::mat4 model = GetModelToWorldMatrix(pos, rot, scl);
		glm::mat4 mvp = proj * view * model;

		Shader* shader = data.material->GetShader();
		Mesh* mesh = data.mesh;
		shader->Use();
		shader->SetMat4("uMVP", mvp);
		data.material->ApplyUniforms(ren.isWireframe);

		GLint loc = glGetUniformLocation(data.material->GetShader()->GetID(), "uMVP");
		std::cout << "uMVP loc=" << loc << "\n";

		// Change the polygon mode to wireframe
		if (ren.isWireframe) { 
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
			mesh->Draw();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			mesh->Draw();
		}
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) std::cerr << "GL ERROR: " << err << "\n";
	}
}

inline void GraphicsSystem2::Initialize() {
	// Default Shader;
	auto shader = std::make_unique<Shader>("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
	AddShader("default", std::move(shader));

	// Textures;
	auto bananaTex = std::make_unique<Texture>("assets/banana.jpg");
	AddTexture("banana", std::move(bananaTex));
	auto targetTex = std::make_unique<Texture>("assets/image_UI_target.png");
	AddTexture("target", std::move(targetTex));

	// Meshes;
	AddMesh("point", Mesh::CreatePoint());
	//AddMesh("line", Mesh::CreateLine());
	AddMesh("square", Mesh::CreateSquare());
	AddMesh("circle", Mesh::CreateCircle());

	// Materials;
	auto bananaMat = std::make_unique<Material>(GetShader("default"), GetTexture("banana"));
	AddMaterial("bananaMat", std::move(bananaMat));
	auto targetMat = std::make_unique<Material>(GetShader("default"), GetTexture("target"));
	AddMaterial("targetMat", std::move(targetMat));

	auto redMat = std::make_unique<Material>(GetShader("default"));
	redMat->SetBaseColor(1.f, 0.f, 0.f, 1.f);
	AddMaterial("redMat", std::move(redMat));

}

// ------------------------------------------------------------------------------------------------
inline void GraphicsSystem2::AddShader(std::string const& name, std::unique_ptr<Shader> shader) {
	m_shaders[name] = std::move(shader);
}
inline void GraphicsSystem2::AddTexture(std::string const& name, std::unique_ptr<Texture> tex) {
	m_textures[name] = std::move(tex);
}
inline void GraphicsSystem2::AddMesh(std::string const& name, std::unique_ptr<Mesh> mesh) {
	m_meshes[name] = std::move(mesh);
}
inline void GraphicsSystem2::AddMaterial(std::string const& name, std::unique_ptr<Material> mat) {
	m_materials[name] = std::move(mat);
}

// ------------------------------------------------------------------------------------------------
inline Shader* GraphicsSystem2::GetShader(std::string const& n) const {
	auto it = m_shaders.find(n);
	return (it != m_shaders.end()) ? it->second.get() : nullptr;
}
inline Texture* GraphicsSystem2::GetTexture(std::string const& n) const {
	auto it = m_textures.find(n);
	return (it != m_textures.end()) ? it->second.get() : nullptr;
}
inline Mesh* GraphicsSystem2::GetMesh(std::string const& n) const {
	auto it = m_meshes.find(n);
	return (it != m_meshes.end()) ? it->second.get() : nullptr;
}
inline Material* GraphicsSystem2::GetMaterial(std::string const& n) const {
	auto it = m_materials.find(n);
	return (it != m_materials.end()) ? it->second.get() : nullptr;
}

// ------------------------------------------------------------------------------------------------
inline void GraphicsSystem2::ResolveRenderable(Entity e, Renderable2 const& r) {
	CachedRenderData cache;
	cache.mesh = GetMesh(r.meshName);
	cache.material = GetMaterial(r.materialName);
	cache.valid = (cache.mesh && cache.material);
	m_renderCache[e] = cache;
}

inline Entity GraphicsSystem2::FindCamera(Coordinator& Coord) {
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