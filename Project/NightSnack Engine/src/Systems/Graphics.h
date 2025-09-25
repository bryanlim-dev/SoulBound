#pragma once

#include <iostream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Systems/Input.h"

#include <glm/gtc/matrix_transform.hpp>


class GraphicsSystem : public ISystem {

public:
	// Default constructor to set up SystemSignature;
	GraphicsSystem();

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

	glm::mat4 GetCameraProjectionMatrix(float width, float height);
	glm::mat4 GetModelToWorldMatrix(Position position, Rotation rotation, Scale scale);

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
	void ResolveRenderable(Entity e, Renderable const& r);
	Entity FindCamera(Coordinator& Coord);

	Signature m_systemSignature{};

};
