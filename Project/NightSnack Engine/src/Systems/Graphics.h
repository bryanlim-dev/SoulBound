/******************************************************************************
 * File:        Graphics.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Graphics System
 *
 * Description:
 * This file contains the declarations of the Graphics System.
 * It manages resources (shaders, textures, meshes, materials), prepares transformation matrices, 
 * applies uniforms, and issues draw calls for each renderable entity in the scene.
 * It also provides debug drawing visualization (circle and AABB collider outlines, velocity vector) 
 * and basic scaling and rotating of a specific object via input events from the event dispatcher. 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Core/CrashLogger.h"
#include "Systems/Application/IEvent.h"

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace NightSnack {

	class GraphicsSystem : public ISystem {

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor for the GraphicsSystem
		Sets up the system signatures so that this system only operates on entities
		that contain components (Position, Rotation, Scale, Renderable)

		\param[in] coord
		Reference to the ECS Coordinator

		\param[in] dispatcher
		Reference to the event dispatcher used for Input Events
		*/
		/************************************************************************/
		GraphicsSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
			// The Graphics System only cares about entities with these components
			// - Position, Rotation, Scale
			// - Renderable (mesh, material, texture, color)
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Renderable>());

			// Initialize viewport dimensions;
			Hvp = static_cast<float>(coord.GetWindowHeight());
			Wvp = static_cast<float>(coord.GetWindowWidth());

			Init();

			// Dispatcher lambda
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				// This input event currently only handles key inputs for
				// - scaling of an object (A / D)
				// - rotating of an object (R)
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::ScaleUp:
						ModifyEntityScale(coord, 1.1f);
						break;
					case InputEvent::InputAction::ScaleDown:
						ModifyEntityScale(coord, 0.9f);
						break;
					case InputEvent::InputAction::Rotate:
						ModifyEntityRotation(coord, 30.f);
						break;
					case InputEvent::InputAction::ToggleDebugDraw:
						m_enableDebugDraw = !m_enableDebugDraw;
						break;
					case InputEvent::InputAction::ToggleInstancedRendering:
						m_enableInstancedRendering = !m_enableInstancedRendering;
						break;
					case InputEvent::InputAction::CrashTest:
						CrashLogger::ForceCrash("Forced crash from GraphicsSystem for testing");
						break;
					case InputEvent::InputAction::TextureTest:
						TestSwapTextures();
						break;
					}
				}
			});
			AddLayer(LayerType::BACKGROUND);
			AddLayer(LayerType::GAMELAYER);
			AddLayer(LayerType::FOREGROUND);
			AddLayer(LayerType::GAMEUI);

			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		}

		/************************************************************************/
		/*!
		\brief
		Initialize function
		All the different types of shaders, textures, meshes, materials are created here
		Subscribing to Input Events from the Event Dispatcher is done here too

		\param[in] Coord
		Reference to the ECS Coordinator
		*/
		/************************************************************************/
		void Init();
		void TestSwapTextures();
		/************************************************************************/
		/*!
		\brief
		Main update called by the core

		\param[in] dt
		Delta time

		\param[in] coord
		Reference to the ECS Coordinator
		*/
		/************************************************************************/
		void Update(int steps) override;

		void RenderText(FT_Face font, const std::string& text, glm::vec2 position, float size, glm::vec4 color, float rotation = 0.f);

		/************************************************************************/
		/*!
		\brief
		Render the scene using the specified camera
		*/
		/************************************************************************/
		void RenderWithCamera(Entity Camera);

		/************************************************************************/
		/*!
		\brief
		Adds a new shader resource to the Graphics System

		\param[in] name
		Key name for lookup

		\param[in] shader
		Unique pointer to Shader
		*/
		/************************************************************************/
		void AddShader(std::string const& name, std::unique_ptr<Shader> shader);

		/************************************************************************/
		/*!
		\brief
		Adds a new texture resource to the Graphics System

		\param[in] name
		Key name for lookup

		\param[in] tex
		Unique pointer to Texture
		*/
		/************************************************************************/
		void AddTexture(std::string const& name, std::unique_ptr<Texture> tex);

		/************************************************************************/
		/*!
		\brief
		Adds a new mesh resource to the Graphics System

		\param[in] name
		Key name for lookup

		\param[in] mesh
		Unique pointer to Mesh
		*/
		/************************************************************************/
		void AddMesh(std::string const& name, std::unique_ptr<Mesh> mesh);

		/************************************************************************/
		/*!
		\brief
		Adds a new material resource to the Graphics System

		\param[in] name
		Key name for lookup

		\param[in] material
		Unique pointer to Material
		*/
		/************************************************************************/
		void AddMaterial(std::string const& name, std::unique_ptr<Material> material);

		/************************************************************************/
		/*!
		\brief
		Builds and returns the camera's projection matrix (orthographic)

		\param[in] width
		Viewport width

		\param[in] height
		Viewport height

		\param[in] zoom
		Zoom level

		\return
		Orthographic projection matrix
		*/
		/************************************************************************/
		glm::mat4 GetCameraProjectionMatrix(float width, float height, float zoom);

		/************************************************************************/
		/*!
		\brief
		Builds and returns the Model-to-World transformation matrix

		\param[in] position
		Entity's position component

		\param[in] rotation
		Entity's rotation component

		\param[in] scale
		Entity's scale component

		\param[in] layer
		Entity's render layer, used to compute z-position

		\return
		Model-to-World transformation matrix
		*/
		/************************************************************************/
		glm::mat4 GetModelToWorldMatrix(Vec3 position, float rotation, Vec2 scale, RenderLayer layer);

		void UpdateTextureOfMaterial(const std::string& textureFilePath, Entity e);

		std::string GetNewMaterialName(std::string oldMatName);

		/************************************************************************/
		/*!
		\brief
		Retrieves a shader by name

		\param[in] n
		Key name for lookup

		\return
		Pointer to the Shader resource
		*/
		/************************************************************************/
		Shader* GetShader(std::string const& n)   const;	// Using raw pointers as the resource belongs to the System...

		/************************************************************************/
		/*!
		\brief
		Retrieves a texture by name

		\param[in] n
		Key name for lookup

		\return
		Pointer to the Texture resource
		*/
		/************************************************************************/
		Texture* GetTexture(std::string const& n)  const;

		/************************************************************************/
		/*!
		\brief
		Retrieves a mesh by name

		\param[in] n
		Key name for lookup

		\return
		Pointer to the Mesh resource
		*/
		/************************************************************************/
		Mesh* GetMesh(std::string const& n)     const;

		/************************************************************************/
		/*!
		\brief
		Retrieves a material by name

		\param[in] n
		Key name for lookup

		\return
		Pointer to the Material resource
		*/
		/************************************************************************/
		Material* GetMaterial(std::string const& n) const;

		/************************************************************************/
		/*!
		\brief
		Finds the active main camera entity in the scene

		\return
		Entity ID of the camera
		*/
		/************************************************************************/
		Entity FindCamera();

		/************************************************************************/
		/*!
		\brief
		Removes cached rendering data for an entity

		\param[in] e
		The entity to remove from cache
		*/
		/************************************************************************/
		void RemoveCache(Entity e);

		const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTexturesMap() const { return m_textures; }

		/************************************************************************/
		/*!
		\brief
		Get the system's signature.
		Required by ISystem inheritance

		\return
		Signature (set of required components)
		*/
		/************************************************************************/
		Signature GetSignature() const override { return m_systemSignature; }

		/************************************************************************/
		/*!
		\brief
		Gets the name of this system, for debugging purposes
		Required by ISystem inheritance

		\return
		Name of the system
		*/
		/************************************************************************/
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

		struct InstanceBatch {
			Mesh* mesh{ nullptr };
			Material* material{ nullptr };
			bool isWireframe{ false };
			std::vector<Mesh::InstanceData> instances;
		};

		// Resource stores (owned by system);
		std::unordered_map<std::string, std::unique_ptr<Shader>>   m_shaders;
		std::unordered_map<std::string, std::unique_ptr<Texture>>  m_textures;
		std::unordered_map<std::string, std::unique_ptr<Mesh>>     m_meshes;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

		std::unordered_map<std::string, InstanceBatch> m_meshBatches;

		/************************************************************************/
		/*!
		\brief
		Resolves a Renderable component into cached render data (mesh + material).
		Looks up the mesh and material resources referenced by the Renderable component
		of an entity. It caches those resolved pointers in m_renderCache.

		\param[in] e
		The entity whose renderable component is being processed

		\param[in] r
		The Renderable component, which contains references (names or IDs)
		to the mesh and material resources used by the entity.
		*/
		/************************************************************************/
		void ResolveRenderable(Entity e, Renderable const& r);

		/************************************************************************/
		/*!
		\brief
		Compute the Z-position value based on the given render layer

		\param[in] layer
		The render layer of an entity

		\return
		A Z-position that can be applied in the model matrix
		*/
		/************************************************************************/
		float GetRenderLayerZPos(RenderLayer layer);

		/************************************************************************/
		/*!
		\brief
		Modifies the scale of an entity's transform

		\param[in] Coord
		Reference to the ECS Coordinator

		\param[in] value
		The scaling value to be used to multiply with its current scale
		*/
		/************************************************************************/
		void ModifyEntityScale(Coordinator& Coord, float value);

		/************************************************************************/
		/*!
		\brief
		Modifies the rotation of an entity's transform

		\param[in] Coord
		Reference to the ECS Coordinator

		\param[in] value
		The rotation value to be used to add to its current rotation
		*/
		/************************************************************************/
		void ModifyEntityRotation(Coordinator& Coord, float value);

		/************************************************************************/
		/*!
		\brief
		Converts screen-space coordinates (mouse position etc) into world-space
		coordinates within the current camera projection.

		Useful to use for interacting with the game world using input devices.

		\param[in] screenPos
		A position in screen coordinates, the mouse cursor location.

		\return
		A Position in world coordinates
		*/
		/************************************************************************/
		Vec3 ScreenToWorld(Vec3 const& screenPos);

		bool m_enableDebugDraw{ false };
		bool m_enableInstancedRendering{ false };
		GLuint m_globalInstanceSSBO = 0;
		GLsizeiptr m_globalInstanceSSBOSize = 0;

		// Viewport dimensions;
		float Hvp, Wvp;

		Signature m_systemSignature{};
		// Reference to Coordinator;
		Coordinator& m_coord;
		// System event dispatcher
		EventDispatcher* m_dispatcher = nullptr;
	};

}	// NightSnack
