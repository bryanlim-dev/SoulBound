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
 * The Graphics System supports both Instanced and Non-Instanced Rendering. 
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
#include <filesystem>

namespace NightSnack {

	class GraphicsSystem : public ISystem {

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor for the GraphicsSystem
		Sets up the system signatures so that this system only operates on entities
		that contain components (Transform, Renderable)

		\param[in] coord
		Reference to the ECS Coordinator

		\param[in] dispatcher
		Reference to the event dispatcher used for Input Events
		*/
		/************************************************************************/
		GraphicsSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
			// The Graphics System only cares about entities with these components
			// - Transform (Position, Rotation, Scale, Layer)
			// - Renderable (Mesh, Material, Texture, Color)
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Renderable>());

			// Initialize viewport dimensions;
			Hvp = static_cast<float>(coord.GetWindowHeight());
			Wvp = static_cast<float>(coord.GetWindowWidth());
			lastHvp = Hvp;
			lastWvp = Wvp;

			//std::string path = std::filesystem::current_path().string();
			//std::cout << path << '\n';

			Init();

			// Dispatcher lambda
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				// This input event currently only handles key inputs for
				// - scaling of an object (A / D)
				// - rotating of an object (R)
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					//case InputEvent::InputAction::ScaleUp:
					//	ModifyEntityScale(coord, 1.1f);
					//	break;
					//case InputEvent::InputAction::ScaleDown:
					//	ModifyEntityScale(coord, 0.9f);
					//	break;
					//case InputEvent::InputAction::Rotate:
					//	ModifyEntityRotation(coord, 30.f);
					//	break;
					case InputEvent::InputAction::ToggleDebugDraw:
						m_enableDebugDraw = !m_enableDebugDraw;
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
		*/
		/************************************************************************/
		void Init();

		/************************************************************************/
		/*!
		\brief
		Main update called by the core
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Render the scene using the specified camera

		\param[in] Camera
		The specified camera that is used to render the scene
		*/
		/************************************************************************/
		void RenderWithCamera(Entity Camera);

		/************************************************************************/
		/*!
		\brief
		Render debug drawing visualizations using the specified camera

		\param[in] Camera
		The specified camera that is used to render the debug drawing visualizations
		*/
		/************************************************************************/
		void RenderDebugDrawing(Entity camera);

		/************************************************************************/
		/*!
		\brief
		Dynamically load all textures at the start of the program via the Resource Manager. 
		*/
		/************************************************************************/
		void LoadTextures();

		/************************************************************************/
		/*!
		\brief
		Recursively iterate through folders, to load all textures within the given folder. 

		\param[in] folderPath
		The current file path of a folder
		*/
		/************************************************************************/
		void IterateFolder(const std::filesystem::path& folderPath);

		/************************************************************************/
		/*!
		\brief
		Creates a texture using the Resource Manager.

		\param[in] textureFilePath
		The texture file path to load
		*/
		/************************************************************************/
		void CreateTexture(std::string textureFilePath);

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
		glm::mat4 GetModelToWorldMatrix(Vec3 position, float rotation, Vec2 scale);

		/************************************************************************/
		/*!
		\brief
		Updates the texture that will be binded to the material that belongs to this Entity.
		This function allows the functionality to change the texture of an Entity in real time, 
		using the level editor. 
		The texture can be either an existing texture, or a new texture to be loaded in. 

		\param[in] textureFilePath
		The file path of the texture that will be binded.

		\param[in] e
		The current entity that will have its texture modified.
		*/
		/************************************************************************/
		void UpdateTextureOfMaterial(const std::string& textureFilePath, Entity e);

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
		Entity GetCamera() const { return m_camera; }

		/************************************************************************/
		/*!
		\brief
		Removes cached rendering data for an entity

		\param[in] e
		The entity to remove from cache
		*/
		/************************************************************************/
		void RemoveCache(Entity e);

		/************************************************************************/
		/*!
		\brief
		Gets the result of whether the program is using Instanced Rendering or not
		This function is called by the ApplicationSystem

		\return
		The result of whether the program is using Instanced Rendering or not
		*/
		/************************************************************************/
		const bool GetEnableInstancedRendering() const { return m_enableInstancedRendering; }

		/************************************************************************/
		/*!
		\brief
		Gets the std::unordered_map of textures from the Graphics System
		This function is called by the AnimationSystem

		\return
		The std::unordered_map of textures from the Graphics System
		*/
		/************************************************************************/
		const std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTexturesMap() const { return m_textures; }

		/************************************************************************/
		/*!
		\brief
		Gets the std::unordered_map of shaders from the Graphics System
		This function is called by the TextSystem

		\return
		The std::unordered_map of shaders from the Graphics System
		*/
		/************************************************************************/
		const std::unordered_map<std::string, std::unique_ptr<Shader>>& GetShadersMap() const { return m_shaders; }

		/************************************************************************/
		/*!
		\brief
		Get all the material keys that are currently stored in the Graphics System. 
		To be used in the level editor, inspector panel, where the user can choose which material to use. 

		\return
		A vector of strings, representing all the material keys that are currently stored in the Graphics System
		*/
		/************************************************************************/
		const std::vector<std::string> GetMaterialMapStrings() const;

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
			LayerType layerType = LayerType::GAMELAYER;
			size_t ssboOffset{ 0 };
			size_t instanceCount{ 0 };
		};

		// Resource stores (owned by system);
		std::unordered_map<std::string, std::unique_ptr<Shader>>   m_shaders;
		std::unordered_map<std::string, std::unique_ptr<Texture>>  m_textures;
		std::unordered_map<std::string, std::unique_ptr<Mesh>>     m_meshes;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

		std::map<std::string, InstanceBatch> m_meshBatches; // Holds all the different InstanceBatch combinations
		std::vector<Mesh::InstanceData> m_allInstances; // Holds all the InstanceData only, to upload to the GPU once

		Entity m_camera;

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
		void ResolveRenderable(Entity e, Renderable& r);

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

		/************************************************************************/
		/*!
		\brief
		Get whether the screen size has changed. 

		\return
		Screen size has changed or not
		*/
		/************************************************************************/
		bool ScreenSizeChanged() const noexcept;

		/************************************************************************/
		/*!
		\brief
		Translate and scale all UI entities accordingly to the new screen resolution. 

		\param[in] mainCam
		The current main camera in the scene
		*/
		/************************************************************************/
		void TranslateUIEntities(Entity mainCam);

		/************************************************************************/
		/*!
		\brief
		Print Debug information to the console.
		*/
		/************************************************************************/
		void DebugRenderableInformation();

		bool m_enableDebugDraw{ false };
		bool m_enableInstancedRendering{ true };
		GLuint m_globalInstanceSSBO = 0;
		GLsizeiptr m_globalInstanceSSBOSize = 0;

		// Viewport dimensions;
		float Hvp, Wvp;
		float lastHvp, lastWvp;

		Signature m_systemSignature{};
		// Reference to Coordinator;
		Coordinator& m_coord;
		// System event dispatcher
		EventDispatcher* m_dispatcher = nullptr;
	};

}	// NightSnack
