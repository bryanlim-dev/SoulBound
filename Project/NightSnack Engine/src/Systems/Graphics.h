#pragma once

#include <iostream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Systems/IEvent.h"

#include <glm/gtc/matrix_transform.hpp>


class GraphicsSystem : public ISystem {

public:
	// Default constructor to set up SystemSignature;
	GraphicsSystem(Coordinator& coord) : m_coord{ coord } {
		// The Graphics System only cares about entities with these components
		// - Position, Rotation, Scale
		// - Renderable (mesh, material, texture, color)
		m_systemSignature.set(GetComponentTypeID<Position>());
		m_systemSignature.set(GetComponentTypeID<Rotation>());
		m_systemSignature.set(GetComponentTypeID<Scale>());
		m_systemSignature.set(GetComponentTypeID<Renderable>());

		// Initialize viewport dimensions;
		Hvp = static_cast<float>(coord.GetWindowHeight());
		Wvp = static_cast<float>(coord.GetWindowWidth());
	}

	void Init(Coordinator& Coord, EventDispatcher& dispatcher);

	// Main update called by core
	void Update(float dt, Coordinator& coord) override;

	// Helper functions for Resource Creation
	void AddShader(std::string const& name, std::unique_ptr<Shader> shader);
	void AddTexture(std::string const& name, std::unique_ptr<Texture> tex);
	void AddMesh(std::string const& name, std::unique_ptr<Mesh> mesh);
	void AddMaterial(std::string const& name, std::unique_ptr<Material> material);

	glm::mat4 GetCameraProjectionMatrix(float width, float height);
	glm::mat4 GetModelToWorldMatrix(Position position, Rotation rotation, Scale scale, RenderLayer layer);

	// Resource Lookup -----------------------------------------------
	Shader*		GetShader(std::string const& n)   const;	// Using raw pointers as the resource belongs to the System...
	Texture*	GetTexture(std::string const& n)  const;
	Mesh*		GetMesh(std::string const& n)     const;
	Material*	GetMaterial(std::string const& n) const;

	Entity FindCamera();
	void RemoveCache(Entity e);

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

	int m_selectedEntityIndex = -1;

	// Helper functions;
	void ResolveRenderable(Entity e, Renderable const& r);
	float GetRenderLayerZPos(RenderLayer layer);
	void ModifyEntityScale(Coordinator& Coord, float value);
	void ModifyEntityRotation(Coordinator& Coord, float value);
	Position ScreenToWorld(Position const& screenPos);

	// DEBUG DRAWING
	bool m_debug{ false };

	Signature m_systemSignature{};

	// Viewport dimensions;
	float Hvp, Wvp;

	// Reference to Coordinator;
	Coordinator& m_coord;
	// System event dispatcher
	EventDispatcher* m_dispatcher = nullptr;
};
