/******************************************************************************
 * File:        Graphics.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Graphics System
 *
 * Description:
 * This file contains the definitions of the Graphics System.
 * It manages resources (shaders, textures, meshes, materials), prepares transformation matrices,
 * applies uniforms, and issues draw calls for each renderable entity in the scene.
 * It also provides debug drawing visualization (circle and AABB collider outlines, velocity vector)
 * and basic scaling and rotating of a specific object via input events from the event dispatcher.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Graphics.h"
#include "MathLibrary/Math.h"

namespace NightSnack {

	void GraphicsSystem::Init() {
		// Default Shader;
		auto debugShader = std::make_unique<Shader>("assets/shaders/debug_vertex_shader.vert", "assets/shaders/debug_fragment_shader.frag");
		AddShader("debug", std::move(debugShader));
		auto colorShader = std::make_unique<Shader>("assets/shaders/color_vs.vert", "assets/shaders/color_fs.frag");
		AddShader("color", std::move(colorShader));
		auto textureShader = std::make_unique<Shader>("assets/shaders/texture_vs.vert", "assets/shaders/texture_fs.frag");
		AddShader("texture", std::move(textureShader));
		auto textShader = std::make_unique<Shader>("assets/shaders/text_vs.vert", "assets/shaders/text_fs.frag");
		AddShader("text", std::move(textShader));

		std::shared_ptr<ResourceManager> rm = m_coord.GetSystem<ResourceManager>();
		// Textures;
		// The number of columns and rows passed in here
		// determines the number of AnimationClips and AnimationFrames
		// [if the entity has an Animator component]
		auto bananaTex = std::make_unique<Texture>("assets/banana.jpg", rm, 4, 1);
		AddTexture("assets/banana.jpg", std::move(bananaTex));
		//auto targetTex = std::make_unique<Texture>("assets/image_UI_target.png", rm, 4, 1);
		//AddTexture("assets/image_UI_target.png", std::move(targetTex));
		auto bgTex = std::make_unique<Texture>("assets/background.png", rm);
		AddTexture("assets/background.png", std::move(bgTex));
		auto ghostTex = std::make_unique<Texture>("assets/ghost.png", rm, 4, 1);
		AddTexture("assets/ghost.png", std::move(ghostTex));
		//auto slimeTex = std::make_unique<Texture>("assets/slime.png", rm, 4, 1);
		//AddTexture("assets/slime.png", std::move(slimeTex));

		// Meshes;
		AddMesh("point", Mesh::CreatePoint());
		AddMesh("line", Mesh::CreateLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f))); // Default facing upwards
		AddMesh("square", Mesh::CreateSquare());
		AddMesh("circle", Mesh::CreateCircle());
		AddMesh("AABBColliderMesh", Mesh::CreateAABBMesh());
		AddMesh("CircleColliderMesh", Mesh::CreateCircleMesh());

		// Materials with texture
		auto bananaMat = std::make_unique<Material>(GetShader("texture"), GetTexture("assets/banana.jpg"));
		bananaMat->SetBaseColor(1.f, 1.f, 1.f, 1.f);
		AddMaterial("bananaMat", std::move(bananaMat));
		//auto targetMat = std::make_unique<Material>(GetShader("texture"), GetTexture("assets/image_UI_target.png"));
		//AddMaterial("targetMat", std::move(targetMat));
		auto backgroundMat = std::make_unique<Material>(GetShader("texture"), GetTexture("assets/background.png"));
		AddMaterial("backgroundMat", std::move(backgroundMat));
		auto ghostMat = std::make_unique<Material>(GetShader("texture"), GetTexture("assets/ghost.png"));
		AddMaterial("ghostMat", std::move(ghostMat));
		//auto slimeMat = std::make_unique<Material>(GetShader("texture"), GetTexture("assets/slime.png"));
		//AddMaterial("slimeMat", std::move(slimeMat));

		/*auto bananaMat = std::make_unique<Material>(GetShader("color"));
		bananaMat->SetBaseColor(1.f, 1.f, 1.f, 1.f);
		AddMaterial("bananaMat", std::move(bananaMat));
		auto targetMat = std::make_unique<Material>(GetShader("color"));
		AddMaterial("targetMat", std::move(targetMat));
		auto backgroundMat = std::make_unique<Material>(GetShader("color"));
		AddMaterial("backgroundMat", std::move(backgroundMat));
		auto ghostMat = std::make_unique<Material>(GetShader("color"));
		AddMaterial("ghostMat", std::move(ghostMat));
		auto slimeMat = std::make_unique<Material>(GetShader("color"));
		AddMaterial("slimeMat", std::move(slimeMat));*/

		// Materials with solid color
		auto colorMat = std::make_unique<Material>(GetShader("color"));
		colorMat->SetBaseColor(1.f, 1.f, 1.f, 1.f);
		AddMaterial("ColorMat", std::move(colorMat));

		// Initialise buffer for Instanced Rendering
		if (m_globalInstanceSSBO == 0) {
			glCreateBuffers(1, &m_globalInstanceSSBO);
			m_globalInstanceSSBOSize = sizeof(Mesh::InstanceData) * NightSnack::MAX_ENTITIES;
			glNamedBufferStorage(m_globalInstanceSSBO, m_globalInstanceSSBOSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_globalInstanceSSBO);
			//std::cout << "Size of InstanceData: " << sizeof(Mesh::InstanceData);
		}
	}

	void GraphicsSystem::Update(int steps) {
		(void)steps;

		if (m_entities.empty()) return;

		// Update viewport dimensions;
		Hvp = static_cast<float>(m_coord.GetWindowHeight());
		Wvp = static_cast<float>(m_coord.GetWindowWidth());

		// Camera stuff...
		Entity mainCam = FindCamera();
		if (mainCam == INVALID_ENTITY) return;

		RenderWithCamera(mainCam);
	}

	void GraphicsSystem::RenderWithCamera(Entity camera) {
		// Get camera components
		auto& camPos = m_coord.GetComponent<Transform>(camera).pos;
		auto& camInfo = m_coord.GetComponent<Camera>(camera);

		glm::mat4 proj = GetCameraProjectionMatrix(camInfo.width, camInfo.height, camInfo.zoom);
		glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camPos.x, -camPos.y, -camPos.z));

		m_meshBatches.clear();

		for (Layer& l : m_layers) {
			if (l.Empty() || !l.IsVisible()) { continue; }
			for (Entity e : l.Entities()) {
				// Reached the end of Valid Entities
				if (e == INVALID_ENTITY) { break; }

				// Obtain entity components
				auto& ren = m_coord.GetComponent<Renderable>(e);
				auto& pos = m_coord.GetComponent<Transform>(e).pos;
				auto& rot = m_coord.GetComponent<Transform>(e).rot;
				auto& scl = m_coord.GetComponent<Transform>(e).scale;

				if (ren.wasModified) { ResolveRenderable(e, ren); }
				// Find entity's renderCache;
				auto it = m_renderCache.find(e);
				if (it == m_renderCache.end() || !it->second.valid) {
					ResolveRenderable(e, ren);
					it = m_renderCache.find(e);
				}

				CachedRenderData& data = it->second;
				if (!data.valid) {
					std::cout << "Entity " << e << " skipped (invalid renderable)\n";
					continue;
				}

				// Creation of Instance Data for Instanced Rendering
				if (m_enableInstancedRendering) {

					// Prepare instance data
					Mesh::InstanceData instance{};

					// Base z offset from the RenderLayer
					float layerZ = GetRenderLayerZPos(ren.renderLayer);
					// Final z offset of this renderable
					float z = layerZ + pos.z;

					instance.pos_rot = glm::vec4{ pos.x, pos.y, z, glm::radians(rot) };
					instance.scale = glm::vec4{ scl.x, scl.y, 1.f, 1.f };

					if (m_coord.HasComponent<Animator>(e)) {
						auto& animator = m_coord.GetComponent<Animator>(e);
						instance.uvRect = glm::vec4{ animator.uvTopLeft.x, animator.uvTopLeft.y,
							animator.uvBottomRight.x, animator.uvBottomRight.y };
					} else {
						instance.uvRect = glm::vec4{ 0.f, 0.f, 1.f, 1.f };
					}

					std::string key = ren.meshName;

					if (m_coord.HasComponent<Color>(e)) {
						auto& col = m_coord.GetComponent<Color>(e);
						instance.color = glm::vec4{ col.r, col.g, col.b, col.a };
						key += "_color";
					} else {
						key = key + "_textured_" + ren.materialName;
					}

					if (ren.isWireframe) {
						key += "_wireframe";
					} else {
						key += "_solid";
					}

					m_meshBatches[key].instances.push_back(instance);
					m_meshBatches[key].mesh = data.mesh;
					m_meshBatches[key].material = data.material;
					m_meshBatches[key].isWireframe = ren.isWireframe;

				} else { // Non-Instanced Rendering
					glm::mat4 model = GetModelToWorldMatrix(pos, rot, scl, ren.renderLayer);

					Shader* shader = data.material->GetShader();
					Mesh* mesh = data.mesh;

					shader->Use();

					// Passing of variables for SOLID COLOR entities only 
					// Only solid color entities have the component color
					shader->SetMat4("uProj", proj);
					shader->SetMat4("uView", view);
					shader->SetBool("uUseInstancing", false);
					shader->SetMat4("uModelToWorld", model);

					if (m_coord.HasComponent<Color>(e)) {
						// Entities with solid color
						auto& clr = m_coord.GetComponent<Color>(e);
						shader->SetVec3("uColor", glm::vec3{ clr.r, clr.g, clr.b });
					} else {
						// Entities with textures
						if (m_coord.HasComponent<Animator>(e)) {
							auto& animator = m_coord.GetComponent<Animator>(e);
							shader->SetVec4("uUVRect", { animator.uvTopLeft.x, animator.uvTopLeft.y,
								animator.uvBottomRight.x, animator.uvBottomRight.y });
						} else {
							shader->SetVec4("uUVRect", { 0.f, 0.f, 1.f, 1.f });
						}
					}

					data.material->ApplyUniforms(ren.isWireframe);

					/*std::cout << "MATERIAL NAME IS: " << ren.materialName << std::endl;
					if (ren.materialName == "ghostMat") {
						std::cout << "MATERIAL NAME ghostMat has texture: " << data.material->GetTexture()->GetTextureName() << std::endl;

					}*/

					// Change the polygon mode to wireframe
					if (ren.isWireframe) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						mesh->Draw();
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					} else {
						mesh->Draw();
					}

					GLenum err = glGetError();
					if (err != GL_NO_ERROR) std::cerr << "GL ERROR: " << err << "\n";
					glEnable(GL_DEBUG_OUTPUT);
					glDebugMessageCallback([](GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* message, const void*) {
						std::cerr << "[GL] " << message << "\n";
						}, nullptr);
				}

				// Debug Drawing, for entities with no texture
				if (m_enableDebugDraw) {
					// For Debug Drawing, debug shader will be used for simplicity
					Shader* shader = GetShader("debug");
					shader->Use();

					// Drawing of colliders 
					if (m_coord.HasComponent<Collider>(e)) {
						Collider const& col = m_coord.GetComponent<Collider>(e);

						switch (col.shape) {
						case ShapeType::AABB:
						{
							// Model matrix according to its collider's scale
							glm::mat4 debugmodel = GetModelToWorldMatrix(pos, rot, Vec2{ col.width, col.height }, RenderLayer::DEBUG);
							glm::mat4 debugMVP = proj * view * debugmodel;
							shader->SetMat4("uDebugMVP", debugMVP);
							shader->SetVec3("uEntityColor", glm::vec3(0.f, 1.f, 0.f)); // Green color
							GetMesh("AABBColliderMesh")->Draw();
						}
						break;

						case ShapeType::Circle:
						{
							// Model matrix according to its collider's scale
							glm::mat4 debugmodel = GetModelToWorldMatrix(pos, rot, Vec2{ col.radius, col.radius }, RenderLayer::DEBUG);
							glm::mat4 debugMVP = proj * view * debugmodel;
							shader->SetMat4("uDebugMVP", debugMVP);
							shader->SetVec3("uEntityColor", glm::vec3(0.f, 1.f, 0.f)); // Green color
							GetMesh("CircleColliderMesh")->Draw();
						}
						break;

						}
					}

					// Drawing of velocity direction for moving objects
					if (m_coord.HasComponent<RigidBody>(e)) {
						Vec2 const& vel = m_coord.GetComponent<RigidBody>(e).vel;

						glm::vec3 velocity(vel.x, vel.y, 0.f);
						float speed = glm::length(velocity); // Magnitude of the velocity vector

						// Only draw the velocity line if it is moving
						if (speed > 0.f) {
							// Max length of the velocity line is slightly above the scale of the entity
							float maxLength = std::max(scl.x, scl.y) * 1.4f;
							float length = std::min(speed, maxLength);

							glm::vec3 dir = glm::normalize(velocity); // Direction of the velocity vector

							// Compute angle between +ve y-axis and the velocity direction
							// atan2(y, x) gives angle relative to +ve x-axis, so subtract 90 degrees to align with +ve y-axis
							float angle = glm::degrees(std::atan2(dir.y, dir.x) - glm::half_pi<float>());

							// Model matrix according to its new rotation, and y-scale so line size reflects speed
							glm::mat4 debugModel = GetModelToWorldMatrix(pos, angle, Vec2{ 1.f, length }, RenderLayer::DEBUG);
							glm::mat4 debugMVP = proj * view * debugModel;
							shader->SetMat4("uDebugMVP", debugMVP);
							shader->SetVec3("uEntityColor", glm::vec3(0.1f, 0.9f, 0.9f));
							// Points upwards by default, will be rotated accordingly
							GetMesh("line")->Draw();
						}
					}
				}
			}
		}

		// Uploading and Drawing for Instanced Rendering
		if (m_enableInstancedRendering) {
			//std::cout << "Num unique instances total: " << m_meshBatches.size() << std::endl;

			// Upload once per mesh type
			for (auto& [key, instanceBatch] : m_meshBatches) {
				//std::cout << "uploading unique mesh with name: " << meshName;
				//std::cout << " with its instance count: " << instances.size() << std::endl;

				Mesh* mesh = instanceBatch.mesh;
				Material* material = instanceBatch.material;

				Shader* shader = material->GetShader();
				shader->Use();

				// Both solid colors and textures will have these uniforms
				shader->SetMat4("uProj", proj);
				shader->SetMat4("uView", view);
				shader->SetBool("uUseInstancing", true);

				material->ApplyUniforms(false);

				size_t size = instanceBatch.instances.size() * sizeof(Mesh::InstanceData);
				glNamedBufferSubData(m_globalInstanceSSBO, 0, size, instanceBatch.instances.data());
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_globalInstanceSSBO);

				if (instanceBatch.isWireframe) {
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f, -1.0f); // push lines slightly toward camera
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					mesh->DrawInstanced(instanceBatch.instances.size());
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDisable(GL_POLYGON_OFFSET_LINE);
				} else {
					mesh->DrawInstanced(instanceBatch.instances.size());
				}
			}
		}
	}

	// This functions will be deleted in due time hahahaha
	void GraphicsSystem::TestSwapTextures() {
		for (Layer& l : m_layers) {
			if (l.Empty() || !l.IsVisible()) { continue; }
			for (Entity e : l.Entities()) {
				// Reached the end of Valid Entities
				if (e == INVALID_ENTITY) { break; }

				// Obtain entity components
				auto& ren = m_coord.GetComponent<Renderable>(e);
				auto& pos = m_coord.GetComponent<Transform>(e).pos;
				auto& rot = m_coord.GetComponent<Transform>(e).rot;
				auto& scl = m_coord.GetComponent<Transform>(e).scale;

				if (ren.wasModified) { ResolveRenderable(e, ren); }
				// Find entity's renderCache;
				auto it = m_renderCache.find(e);
				if (it == m_renderCache.end() || !it->second.valid) {
					ResolveRenderable(e, ren);
					it = m_renderCache.find(e);
				}

				CachedRenderData& data = it->second;
				if (!data.valid) {
					std::cout << "Entity " << e << " skipped (invalid renderable)\n";
					continue;
				}

				// swap textures
				if (ren.materialName == "ghostMat") {

					//std::cout << "\nK Pressed, Entity " << e << " is a ghostMat\n";
					//std::cout << "Entity " << e << " material name is: " << ren.materialName << std::endl;
					UpdateTextureOfMaterial("assets/slime.png", e);
					return;
				}
				if (ren.materialName == "ghostMat1") {
					//std::cout << "\nK Pressed, Entity " << e << " is a ghostMat1\n";
					//std::cout << "Entity " << e << " material name is: " << ren.materialName << std::endl;
					UpdateTextureOfMaterial("assets/image_UI_target.png", e);
				}
				if (ren.materialName == "ghostMat2") {
					//std::cout << "\nK Pressed, Entity " << e << " is a ghostMat2\n";
					//std::cout << "Entity " << e << " material name is: " << ren.materialName << std::endl;
					UpdateTextureOfMaterial("assets/ghost.png", e);
				}
			}
		}
	}

	void GraphicsSystem::UpdateTextureOfMaterial(const std::string& textureFilePath, Entity e) {
		auto& ren = m_coord.ModifyComponent<Renderable>(e);

		// Check whether the current texture exists
		Texture* tex = GetTexture(textureFilePath);
		if (!tex) {
			// This is a new texture, add it to the resource map
			// Create the Texture pointer
			std::shared_ptr<ResourceManager> rm = m_coord.GetSystem<ResourceManager>();
			auto newTex = std::make_unique<Texture>(textureFilePath, rm, 4, 1);
			AddTexture(textureFilePath, std::move(newTex));

			// If it is a new texture, it means that there is no existing material for it
			// Create new material with this texture combination
			auto newMat = std::make_unique<Material>(GetShader("texture"), GetTexture(textureFilePath));
			std::string newName = GetNewMaterialName(ren.materialName);
			// Name this new material to be a variant of the current material
			AddMaterial(newName, std::move(newMat));

			// Change the name of this Renderable's material name to the new material name
			ren.materialName = newName;
			//std::cout << "New texture detected!\n";
			//std::cout << "Entity " << e << " material changed to " << ren.materialName << std::endl;
			//std::cout << "and this texture name is: " << m_materials[ren.materialName]->GetTexture()->GetTextureName() << std::endl;

		} else {
			// Texture already exists, but Material might not
			// Check whether combination of this Material exists with this Texture
			//std::cout << "Texture exists already!\n";
			bool found = false;
			for (const auto& [matName, material] : m_materials) {
				// Check whether the texture exists first! 
				// Since there are colorMat with no textures too
				Texture* tex = material->GetTexture();
				if (!tex) continue;

				// If it exists already, change the name of this Renderable's material to the existing material name
				//std::cout << "Checking for texture name: " << textureFileName << std::endl;
				if (textureFilePath == tex->GetTextureName()) {
					ren.materialName = matName;
					found = true;
					//std::cout << "Material for this texture exists too!\n";
					break;
				}

			}
			if (!found) {
				//std::cout << "Material for this texture doesnt exist yet, creating one ...\n";
				// if it does not exist, create new material with this texture combination
				auto newMat = std::make_unique<Material>(GetShader("texture"), GetTexture(textureFilePath));
				std::string newName = GetNewMaterialName(ren.materialName);
				// Name this new material to be a variant of the current material
				AddMaterial(newName, std::move(newMat));
				// change the name of this Renderable's material name to the new material name
				ren.materialName = newName;
			}
			//std::cout << "Entity " << e << " material changed to " << ren.materialName << std::endl;
		}
	}

	std::string GraphicsSystem::GetNewMaterialName(std::string oldMatName) {
		// Checks how many variants of the original material name there are, and get the next variant
		// "ghost", "ghost1", "ghost2" etc ...

		// Find the position where the 'number' in the string starts
		size_t len = oldMatName.length();
		size_t i = len;

		// Find the first non-digit character
		while (i > 0 && std::isdigit(oldMatName[i - 1])) {
			--i;
		}

		// If there are digits at the end, extract the number from the string
		if (i < len) {
			std::string baseName = oldMatName.substr(0, i);
			int num = std::stoi(oldMatName.substr(i)); // Convert from string to number

			// Increment the digit by 1
			return baseName + std::to_string(num + 1);
		} else {
			// This is the first variant 
			return oldMatName + "1";
		}
	}

	// Helper functions to add resources --------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------

	void GraphicsSystem::AddShader(std::string const& name, std::unique_ptr<Shader> shader) {
		m_shaders[name] = std::move(shader);
	}
	void GraphicsSystem::AddTexture(std::string const& name, std::unique_ptr<Texture> tex) {
		m_textures[name] = std::move(tex);
	}
	void GraphicsSystem::AddMesh(std::string const& name, std::unique_ptr<Mesh> mesh) {
		m_meshes[name] = std::move(mesh);
	}
	void GraphicsSystem::AddMaterial(std::string const& name, std::unique_ptr<Material> mat) {
		m_materials[name] = std::move(mat);
	}

	// Compute the transformation matrices ------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------

	glm::mat4 GraphicsSystem::GetCameraProjectionMatrix(float width, float height, float zoom) {
		float halfWidth = (width * 0.5f) / zoom;
		float halfHeight = (height * 0.5f) / zoom;
		// Orthographic projection keeps objects the same size regardless of depth
		// Arguments in the order of (left, right, bottom, top, near, far)
		return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, Z_EPSILON, 10.f);
	}

	glm::mat4 GraphicsSystem::GetModelToWorldMatrix(Vec3 position, float rotation, Vec2 scale, RenderLayer layer) {
		glm::mat4 model(1.f);

		// Base z offset from the RenderLayer
		float layerZ = GetRenderLayerZPos(layer);
		// Final z offset of this renderable
		float z = layerZ + position.z;

		model = glm::translate(model, glm::vec3(position.x, position.y, z));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.f, 0.f, 1.f)); // Rotate about z-axis
		model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.f));
		return model;
	}

	// Helper functions to access resources -----------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------

	Shader* GraphicsSystem::GetShader(std::string const& n) const {
		auto it = m_shaders.find(n);
		return (it != m_shaders.end()) ? it->second.get() : nullptr;
	}
	Texture* GraphicsSystem::GetTexture(std::string const& n) const {
		auto it = m_textures.find(n);
		return (it != m_textures.end()) ? it->second.get() : nullptr;
	}
	Mesh* GraphicsSystem::GetMesh(std::string const& n) const {
		auto it = m_meshes.find(n);
		return (it != m_meshes.end()) ? it->second.get() : nullptr;
	}
	Material* GraphicsSystem::GetMaterial(std::string const& n) const {
		auto it = m_materials.find(n);
		return (it != m_materials.end()) ? it->second.get() : nullptr;
	}

	void GraphicsSystem::ResolveRenderable(Entity e, Renderable const& r) {
		CachedRenderData cache;
		cache.mesh = GetMesh(r.meshName);
		cache.material = GetMaterial(r.materialName);
		cache.valid = (cache.mesh && cache.material);
		m_renderCache[e] = cache;
	}

	Entity GraphicsSystem::FindCamera() {
		// Camera Signature (only have components Camera and Tranform)
		Signature cameraSig;
		cameraSig.set(GetComponentTypeID<Camera>());
		cameraSig.set(GetComponentTypeID<Transform>());

		// Iterate through all living Entities in the COORDINATOR instead
		// Since the GraphicsSystem will not have a Camera Entity
		for (const Entity e : m_coord.GetLivingEntities()) {
			if ((m_coord.GetEntitySignature(e) & cameraSig) == cameraSig &&
				m_coord.GetComponent<Camera>(e).isMainCam) { // This camera Entity is the main camera
				return e;
			}
		}
		std::cerr << "Error: No main camera found in scene!\n";
		return INVALID_ENTITY;
	}

	float GraphicsSystem::GetRenderLayerZPos(RenderLayer layer) {

		// Higher values will render on top of lower values
		// Currently, the near and far plane is -0.0001.f and -10.f
		switch (layer) {
		case RenderLayer::BACKGROUND: return -9.5f;	 break;
		case RenderLayer::DEFAULT:	  return -7.f;	 break;
		case RenderLayer::DEBUG:	  return -3.f;	 break;
		case RenderLayer::UI:		  return -2.f;	 break;
		default:					  return -7.f;	 break;
		}
	}

	void GraphicsSystem::ModifyEntityScale(Coordinator& coord, float value) {
		auto it = m_entities.begin();

		// Currently hardcoded to only 'control entity'
		std::advance(it, 3);
		auto& ren = coord.GetComponent<Renderable>(*it);
		if (ren.materialName != "targetMat")
			return;

		auto& scale = coord.ModifyComponent<Transform>(*it).scale;
		scale.x *= value;
		scale.y *= value;
		// In the future, modify the collider too when scaling normal objects
		//auto& collider = coord.ModifyComponent<Collider>(*it);
		//collider.width *= value;
		//collider.height *= value;
		//collider.radius *= value;
	}

	void GraphicsSystem::ModifyEntityRotation(Coordinator& coord, float value) {
		auto it = m_entities.begin();

		// Currently hardcoded to only 'control entity'
		std::advance(it, 3);
		auto& ren = coord.GetComponent<Renderable>(*it);
		if (ren.materialName != "targetMat")
			return;

		auto& rotation = coord.ModifyComponent<Transform>(*it).rot;
		rotation += value;
		if (rotation >= 360.f)
			rotation -= 360.f;
	}

	Vec3 GraphicsSystem::ScreenToWorld(Vec3 const& screenPos) {
		Vec3 pos{};
		// To convert from screen to clip: X -> -Wvp/2 + x; Y -> Hvp/2 - y; 
		pos.x = (-Wvp / 2.f) + screenPos.x;
		pos.y = (Hvp / 2.f) - screenPos.y;

		// Clip to World: take into account CameraPos;
		Entity cam = FindCamera();

		pos.x += m_coord.GetComponent<Transform>(cam).pos.x;
		pos.y += m_coord.GetComponent<Transform>(cam).pos.y;

		return pos;
	}

	void GraphicsSystem::RemoveCache(Entity e) {
		m_renderCache.erase(e);
	}

}	// NightSnack
