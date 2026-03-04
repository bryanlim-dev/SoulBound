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
 * The Graphics System supports both Instanced and Non-Instanced Rendering.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Graphics.h"
#include "MathLibrary/Math.h"

namespace NightSnack {

	void GraphicsSystem::Init() {
		// Default Shader;
		auto debugShader = std::make_unique<Shader>("shaders/debug_vs.vert", "shaders/debug_fs.frag");
		AddShader("debug", std::move(debugShader));
		auto colorShader = std::make_unique<Shader>("shaders/color_vs.vert", "shaders/color_fs.frag");
		AddShader("color", std::move(colorShader));
		auto textureShader = std::make_unique<Shader>("shaders/texture_vs.vert", "shaders/texture_fs.frag");
		AddShader("texture", std::move(textureShader));
		auto textShader = std::make_unique<Shader>("shaders/text_vs.vert", "shaders/text_fs.frag");
		AddShader("text", std::move(textShader));
		auto glowShader = std::make_unique<Shader>("shaders/particle_glow_vs.vert", "shaders/particle_glow_fs.frag");
		AddShader("glow", std::move(glowShader));
		auto invisShader = std::make_unique<Shader>("shaders/invis_vs.vert", "shaders/invis_fs.frag");
		AddShader("invis", std::move(invisShader));

		// Meshes;
		AddMesh("point", Mesh::CreatePoint());
		AddMesh("line", Mesh::CreateLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f))); // Default facing upwards
		AddMesh("square", Mesh::CreateSquare());
		AddMesh("circle", Mesh::CreateCircle());
		AddMesh("AABBColliderMesh", Mesh::CreateAABBMesh());
		AddMesh("CircleColliderMesh", Mesh::CreateCircleMesh());

		// Materials with solid color
		// THE CREATION OF THIS COLOR MATERIAL SHALL BE MOVED TO RESOURCE MANAGER INIT
		auto colorMat = std::make_unique<Material>(GetShader("color"));
		//colorMat->SetBaseColor(1.f, 1.f, 1.f, 1.f);
		AddMaterial("ColorMat", std::move(colorMat));
		auto glowMat = std::make_unique<Material>(GetShader("glow"));
		AddMaterial("GlowMat", std::move(glowMat));
		auto invMat = std::make_unique<Material>(GetShader("invis"));
		AddMaterial("InvisMat", std::move(invMat));

		// Initialise buffer for Instanced Rendering
		if (m_globalInstanceSSBO == 0) {
			glCreateBuffers(1, &m_globalInstanceSSBO);
			m_globalInstanceSSBOSize = sizeof(Mesh::InstanceData) * NightSnack::MAX_ENTITIES;
			//glNamedBufferStorage(m_globalInstanceSSBO, m_globalInstanceSSBOSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

			// Explicit zero-initialization
			std::vector<Mesh::InstanceData> zeroData(NightSnack::MAX_ENTITIES);
			memset(zeroData.data(), 0, m_globalInstanceSSBOSize);
			glNamedBufferStorage(m_globalInstanceSSBO, m_globalInstanceSSBOSize, zeroData.data(), GL_DYNAMIC_STORAGE_BIT);

			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_globalInstanceSSBO);
			////std::cout << "Size of InstanceData: " << sizeof(Mesh::InstanceData);
		}
		glEnable(GL_DEPTH_TEST);
		LoadTextures();
	}

	void GraphicsSystem::Update() {

		if (m_entities.empty()) return;

		// Update viewport dimensions;
		Hvp = static_cast<float>(m_coord.GetWindowHeight());
		Wvp = static_cast<float>(m_coord.GetWindowWidth());

		// Camera stuff...
		Entity mainCam = FindCamera();
		if (mainCam == INVALID_ENTITY) return;

		if (ScreenSizeChanged()) {
			//TranslateUIEntities(mainCam);
			lastHvp = Hvp;
			lastWvp = Wvp;
		}
		TranslateUIEntities(mainCam);
		RenderWithCamera(mainCam);
	}

	void GraphicsSystem::RenderWithCamera(Entity camera) {
		// By default, GL_DEPTH_TEST is disabled for scene objects, since they will be rendered by their Layers order

		// Get camera components
		auto& camPos = m_coord.GetComponent<Transform>(camera).pos;
		auto& camInfo = m_coord.GetComponent<Camera>(camera);

		glm::mat4 proj = GetCameraProjectionMatrix(camInfo.width, camInfo.height, camInfo.zoom);
		glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camPos.x, -camPos.y, -camPos.z));

		m_orderedBatches.clear();
		m_allInstances.clear();

		float layerZ = -2.f;
		for (Layer& l : m_layers) {
			layerZ += 2.f;
			if (l.Empty() || !l.IsVisible() || !l.IsActive()) { continue; }
			for (Entity e : l.Entities()) {
				// Reached the end of Valid Entities
				if (e == INVALID_ENTITY) { break; }

				// Obtain entity components
				auto& ren = m_coord.GetComponent<Renderable>(e);
				auto& trans = m_coord.GetComponent<Transform>(e);
				auto& pos = m_coord.GetComponent<Transform>(e).pos;
				auto& rot = m_coord.GetComponent<Transform>(e).rot;
				auto& scl = m_coord.GetComponent<Transform>(e).scale;

				float normalZ = (pos.z + layerZ) / camPos.z;

				if (!trans.isActive) { continue; }
				if (ren.materialName == "InvisMat") { continue; }

				if (ren.wasModified) {
					//std::cout << "Resolving renderable: " << e << '\n'; 
					ResolveRenderable(e, m_coord.ModifyComponent<Renderable>(e));
				}
				// Find entity's renderCache;
				auto it = m_renderCache.find(e);
				if (it == m_renderCache.end() || !it->second.valid) {
					ResolveRenderable(e, m_coord.ModifyComponent<Renderable>(e));
					it = m_renderCache.find(e);
				}

				CachedRenderData& data = it->second;
				if (!data.valid) {
					//std::cout << "Entity " << e << " skipped (invalid renderable)\n";
					continue;
				}

				// If the layer of this entity has been changed in real-time, 
				// Remove this entity from the previous layer, and insert it into the new layer
				if (trans.layer != l.Type()) {
					l.Remove(e);
					for (auto& layer : m_layers) {
						if (layer.Type() == trans.layer) {
							layer.Insert(e);
							break;
						}
					}
					continue;
				}

				// Creation of Instance Data for Instanced Rendering
				if (m_enableInstancedRendering) {

					// Prepare instance data
					Mesh::InstanceData instance{};
					instance.pos_rot = glm::vec4{ pos.x, pos.y, normalZ, glm::radians(rot) };	//
					instance.scale = glm::vec4{ scl.x, scl.y, 1.f, 1.f };

					// UV coordinates
					if (m_coord.HasComponent<Animator>(e)) {
						auto& animator = m_coord.GetComponent<Animator>(e);
						// Pass the UV coordinates that were updated from the AnimationSystem
						glm::vec4 uv = { animator.uvTopLeft.x, animator.uvTopLeft.y,
											animator.uvBottomRight.x, animator.uvBottomRight.y };
						// Swap the y coordinates for animated entities on the GAMEUI layer
						if (l.Type() == LayerType::GAMEUI) {
							std::swap(uv.y, uv.w);
						}
						instance.uvRect = uv;

					} else {
						// Non-animated textures will use default values
						instance.uvRect = glm::vec4{ 0.f, 0.f, 1.f, 1.f };
					}

					instance.color = glm::vec4{ 1.f, 1.f, 1.f, 1.f };

					// Each different combination of color/texture/wireframe will be sent in different batches
					std::string key = ren.meshName;

					// Entities that are of solid color must have
					// ColorMat as their material and a Color component
					if (ren.materialName == "ColorMat" && m_coord.HasComponent<Color>(e)) {
						// Change the instance color to be based on its Color component values
						auto& col = m_coord.GetComponent<Color>(e);
						instance.color = glm::vec4{ col.r, col.g, col.b, col.a };
						data.material->SetBaseColor(col.r, col.g, col.b, col.a);
						key += "_color";
					} else if (ren.materialName == "GlowMat" && m_coord.HasComponent<Color>(e)) {
						// Change the instance color to be based on its Color component values
						auto& col = m_coord.GetComponent<Color>(e);
						instance.color = glm::vec4{ col.r, col.g, col.b, col.a };
						data.material->SetBaseColor(col.r, col.g, col.b, col.a);
						key += "_glow";
					} else {
						// Entities that have textured materials
						key = key + "_textured_" + ren.materialName;
					}


					if (ren.isWireframe) {
						key += "_wireframe";
					} else {
						key += "_solid";
					}

					// Since for Instanced Rendering, we are using a std::map instead
					// Let background layers render first so they can appear behind
					// For it to render first, their key has to be alphabetically smaller
					switch (l.Type()) {
					case LayerType::_ENGINE:	key = "layer0_" + key;	break;
					case LayerType::BACKGROUND: key = "layer1_" + key;	break;
					default:
					case LayerType::GAMELAYER:	key = "layer2_" + key;	break;
					case LayerType::FOREGROUND: key = "layer3_" + key;	break;
					case LayerType::GAMEUI:		key = "layer4_" + key;	break;
					case LayerType::_ENGINE2:	key = "layer5_" + key;	break;
					}

					InstanceBatch batch = {
						data.mesh,
						data.material,
						ren.isWireframe,
						{ instance },
						l.Type(),
						0,
						0,
						static_cast<int>(l.Type())
					};
					batch.zPos = instance.pos_rot.z;
					m_orderedBatches.push_back(batch);

				} else { // Non-Instanced Rendering
					// Position and scale has been modified before getting the model matrix for UI entities
					Vec3 posz = { pos.x, pos.y, normalZ };
					glm::mat4 model = GetModelToWorldMatrix(posz, rot, scl);		//

					Shader* shader = data.material->GetShader();
					Mesh* mesh = data.mesh;

					shader->Use();


					// Entities on the GAMEUI layer will have different transformatio matrices
					glm::mat4 viewToUse = (l.Type() == LayerType::GAMEUI) ? glm::mat4(1.f) : view;
					glm::mat4 projToUse = (l.Type() == LayerType::GAMEUI) ? glm::ortho(0.f, Wvp, Hvp, 0.f) : proj;

					shader->SetMat4("uView", viewToUse);
					shader->SetMat4("uProj", projToUse);
					shader->SetBool("uUseInstancing", false);
					shader->SetMat4("uModelToWorld", model);
					shader->SetBool("uIsWireframe", ren.isWireframe);

					if (shader == GetShader("glow")) {
						if (m_coord.HasComponent<Particle>(e)) {
							auto& particle = m_coord.GetComponent<Particle>(e);
							shader->SetFloat("uGlowIntensity", particle.glowIntensity);
							shader->SetFloat("uGlowFalloff", particle.glowFalloff);
						} else {
							shader->SetFloat("uGlowIntensity", 1.5f);
							shader->SetFloat("uGlowFalloff", 2.f);
						}
					}

					// Passing of variables for SOLID COLOR entities only 
					// Only solid color entities have the component color
					if (m_coord.HasComponent<Color>(e)) {
						// Entities with solid color
						auto& clr = m_coord.GetComponent<Color>(e);
						shader->SetVec4("uColor", glm::vec4{ clr.r, clr.g, clr.b, clr.a });
						data.material->SetBaseColor(clr.r, clr.g, clr.b, clr.a);
					} else {
						// Entities with textures
						if (m_coord.HasComponent<Animator>(e)) {
							auto& animator = m_coord.GetComponent<Animator>(e);

							glm::vec4 uv = { animator.uvTopLeft.x, animator.uvTopLeft.y,
											animator.uvBottomRight.x, animator.uvBottomRight.y };
							// Swap the y coordinates for animated entities on the GAMEUI layer
							if (l.Type() == LayerType::GAMEUI) {
								std::swap(uv.y, uv.w);
							}
							shader->SetVec4("uUVRect", uv);

						} else {
							shader->SetVec4("uUVRect", { 0.f, 0.f, 1.f, 1.f });
						}
						// Default color is white, if the texture is not initialized
						shader->SetVec4("uColor", glm::vec4{ 1.f, 1.f, 1.f, 1.f });
					}

					data.material->ApplyUniforms(ren.isWireframe);

					if (shader == GetShader("glow")) {
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive blending for the glow effect
					}

					// Change the polygon mode to wireframe
					if (ren.isWireframe) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						mesh->Draw();
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					} else {
						mesh->Draw();
					}

					if (shader == GetShader("glow")) {
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // reset back to normal
						glDisable(GL_BLEND);
					}

					GLenum err = glGetError();
					if (err != GL_NO_ERROR) std::cerr << "GL ERROR: " << err << "\n";
				}
			}
		}

		// Uploading and Drawing for Instanced Rendering
		if (m_enableInstancedRendering) {
			////std::cout << "Num unique instances total: " << m_orderedBatches.size() << std::endl;

			// After collecting all mesh batches, sort them according to layer order
			std::stable_sort(m_orderedBatches.begin(), m_orderedBatches.end(),
				[](const InstanceBatch& a, const InstanceBatch& b) {
					// Sort by different layers
					if (a.layerOrder != b.layerOrder) {
						return a.layerOrder < b.layerOrder;
					}
					// Sort by different z positions
					return a.zPos < b.zPos;
				});

			// Update m_allInstances with the collected InstanceBatches, and the SSBO offsets
			size_t currentOffset{ 0 };
			for (auto& instanceBatch : m_orderedBatches) {
				instanceBatch.ssboOffset = currentOffset;
				instanceBatch.instanceCount = instanceBatch.instances.size();
				m_allInstances.insert(m_allInstances.end(), instanceBatch.instances.begin(), instanceBatch.instances.end());
				currentOffset += instanceBatch.instanceCount;
			}

			// Upload all the InstanceData as one batch per frame
			glNamedBufferSubData(m_globalInstanceSSBO, 0, m_allInstances.size() * sizeof(Mesh::InstanceData), m_allInstances.data());
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_globalInstanceSSBO);

			// Upload uniforms in respect to its InstanceBatch
			for (auto& instanceBatch : m_orderedBatches) {
				//std::cout << "uploading unique mesh with name: " << meshName;
				//std::cout << " with its instance count: " << instances.size() << std::endl;

				Mesh* mesh = instanceBatch.mesh;
				Material* material = instanceBatch.material;

				Shader* shader = material->GetShader();
				shader->Use();

				// Both solid colors and textures will have these uniforms

				// Entities on the GAMEUI layer will have different transformation matrices
				glm::mat4 projToUse = (instanceBatch.layerType == LayerType::GAMEUI) ? glm::ortho(0.f, Wvp, Hvp, 0.f) : proj;
				glm::mat4 viewToUse = (instanceBatch.layerType == LayerType::GAMEUI) ? glm::mat4(1.f) : view;

				shader->SetMat4("uProj", projToUse);
				shader->SetMat4("uView", viewToUse);
				shader->SetBool("uUseInstancing", true);
				shader->SetBool("uIsWireframe", instanceBatch.isWireframe);
				shader->SetInt("uInstanceOffset", static_cast<int>(instanceBatch.ssboOffset));

				if (shader == GetShader("glow")) {
					shader->SetFloat("uGlowIntensity", 1.5f);
					shader->SetFloat("uGlowFalloff", 2.f);
				}

				material->ApplyUniforms(instanceBatch.isWireframe);

				if (shader == GetShader("glow")) {
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive blending for the glow effect
				}

				if (instanceBatch.isWireframe) {
					glDepthMask(GL_TRUE);
					glEnable(GL_POLYGON_OFFSET_LINE);
					glPolygonOffset(-1.0f, -1.0f); // push lines slightly toward camera
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					mesh->DrawInstanced(instanceBatch.instanceCount);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glDisable(GL_POLYGON_OFFSET_LINE);
					glDepthMask(GL_FALSE);
				} else {
					mesh->DrawInstanced(instanceBatch.instanceCount);
				}

				if (shader == GetShader("glow")) {
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // reset back to normal
					glDisable(GL_BLEND);
				}
			}
		}

#ifdef EDITOR
		// Render debug drawing visualizations only after rendering all scene objects
		if (m_enableDebugDraw)
			RenderDebugDrawing(camera);
#endif
	}

	void GraphicsSystem::RenderDebugDrawing(Entity camera) {
		// Debug drawing visualizations will render on top of their own entities
		// Only enable GL_DEPTH_TEST for debug drawing so that we can offset the colliders slightly in front of its entity
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Debug Drawing, for entities with no texture
		// For Debug Drawing, debug shader will be used for simplicity
		Shader* shader = GetShader("debug");
		shader->Use();

		// Get camera components
		auto& camPos = m_coord.GetComponent<Transform>(camera).pos;
		auto& camInfo = m_coord.GetComponent<Camera>(camera);

		// Get the updated window resolution from the ApplicationSystem first
		float screenW = static_cast<float>(m_coord.GetSystem<ApplicationSystem>()->GetWidth());
		float screenH = static_cast<float>(m_coord.GetSystem<ApplicationSystem>()->GetHeight());

		glm::mat4 proj = GetCameraProjectionMatrix(camInfo.width, camInfo.height, camInfo.zoom);
		glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camPos.x, -camPos.y, -camPos.z));

		m_orderedBatches.clear();

		float layerZ = -1.f;
		for (Layer& l : m_layers) {
			layerZ += 1.f;
			if (l.Empty() || !l.IsVisible() || !l.IsActive()) { continue; }
			for (Entity e : l.Entities()) {
				// Reached the end of Valid Entities
				if (e == INVALID_ENTITY) { break; }

				// Obtain entity components
				auto& pos = m_coord.GetComponent<Transform>(e).pos;
				auto& rot = m_coord.GetComponent<Transform>(e).rot;
				auto& scl = m_coord.GetComponent<Transform>(e).scale;

				float normalZ = (pos.z + layerZ) / camPos.z;

				// Entities on the GAMEUI layer will have different transformation matrices
				glm::mat4 viewToUse = (l.Type() == LayerType::GAMEUI) ? glm::mat4(1.f) : view;
				glm::mat4 projToUse = (l.Type() == LayerType::GAMEUI) ? glm::ortho(0.f, screenW, screenH, 0.f) : proj;

				// Drawing of colliders 
				if (m_coord.HasComponent<Collider>(e)) {
					Collider const& col = m_coord.GetComponent<Collider>(e);

					glm::vec3 colliderColor{};
					if (col.colliding) { colliderColor = glm::vec3(1.f, 0.f, 0.f); } // Red color
					else { colliderColor = glm::vec3(0.f, 1.f, 0.f); } // Green color

					switch (col.shape) {
					case ShapeType::AABB:
					{
						// Model matrix according to its collider's scale
						// Make the z value of the collider be slightly in front of its entity
						glm::mat4 debugmodel = GetModelToWorldMatrix({ pos.x, pos.y, normalZ + 0.1f }, rot, Vec2{ col.width, col.height });		//
						glm::mat4 debugMVP = projToUse * viewToUse * debugmodel;
						shader->SetMat4("uDebugMVP", debugMVP);
						shader->SetVec3("uEntityColor", colliderColor);
						GetMesh("AABBColliderMesh")->Draw();
					}
					break;

					case ShapeType::Circle:
					{
						// Model matrix according to its collider's scale
						// Make the z value of the collider be slightly in front of its entity
						glm::mat4 debugmodel = GetModelToWorldMatrix({ pos.x, pos.y, normalZ + 0.1f }, rot, Vec2{ col.radius, col.radius });		//
						glm::mat4 debugMVP = projToUse * viewToUse * debugmodel;
						shader->SetMat4("uDebugMVP", debugMVP);
						shader->SetVec3("uEntityColor", colliderColor);
						GetMesh("CircleColliderMesh")->Draw();
					}
					break;

					}
				}
				else {
					// Draw outline of Live Entities
					glm::vec3 outlineColor(0.5f);
					// Model matrix according to its collider's scale
						// Make the z value of the collider be slightly in front of its entity
					glm::mat4 debugmodel = GetModelToWorldMatrix({ pos.x, pos.y, normalZ + 0.1f }, rot, Vec2{ scl.x, scl.y });				//
					glm::mat4 debugMVP = projToUse * viewToUse * debugmodel;
					shader->SetMat4("uDebugMVP", debugMVP);
					shader->SetVec3("uEntityColor", outlineColor);
					GetMesh("AABBColliderMesh")->Draw();
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
						// Make the z value of the collider be slightly in front of its entity
						glm::mat4 debugModel = GetModelToWorldMatrix({ pos.x, pos.y, normalZ + 0.1f }, angle, Vec2{ 1.f, length });		//
						glm::mat4 debugMVP = proj * view * debugModel;
						shader->SetMat4("uDebugMVP", debugMVP);
						shader->SetVec3("uEntityColor", glm::vec3(0.1f, 0.9f, 0.9f));
						// Points upwards by default, will be rotated accordingly
						GetMesh("line")->Draw();
					}
				}
			}
		}

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
	}

	void GraphicsSystem::LoadTextures() {
		std::filesystem::path resourcePath = std::filesystem::canonical(
			RESOURCE_PATH // "NightSnack Engine/assets"
		);

		//resourcePath /= "Sprites";  // "NightSnack Engine/assets/Sprites"
		resourcePath = resourcePath.lexically_normal();
		
		//std::cout << "Resource path: " << resourcePath << std::endl;

		if (!std::filesystem::exists(resourcePath)) {
			std::cerr << "[GraphicsSystem] Error: Sprites directory cannot be found!\n";
			return;
		}

		// Recursively load all texture files within assets/Sprites folder
		IterateFolder(resourcePath);
	}

	void GraphicsSystem::IterateFolder(const std::filesystem::path& folderPath) {
		//std::cout << "Iterating folder: " << folderPath.string() << std::endl;
		for (auto const& entry : std::filesystem::directory_iterator(folderPath)) {
			// If this entry is a folder, go inside this folder and iterate through it
			if (entry.is_directory()) {
				IterateFolder(entry.path());
				continue;
			}

			// This entry is a file
			std::filesystem::path filePath = entry.path();
			std::string extension = filePath.extension().string();

			// Only load image files with the correct extensions
			if (extension != ".png" && extension != ".jpg" && extension != ".jpeg") {
				continue;
			}

			// Path relative to the asset root
			std::filesystem::path relativePath = std::filesystem::relative(filePath, std::filesystem::canonical(RESOURCE_PATH));
			CreateTexture(relativePath.string());

			// If it is a new texture, it means that there is no existing material for it
			// Create new material with this texture combination
			auto newMat = std::make_unique<Material>(GetShader("texture"), GetTexture(relativePath.string()));
			// Name of the material will be based on the texture file path, but the name only
			AddMaterial(filePath.filename().stem().string(), std::move(newMat));
			//std::cout << "Created Material: " << filePath.filename().stem().string() << std::endl;
		}
	}

	void GraphicsSystem::CreateTexture(std::string textureFilePath) {

		// Texture already exists
		if (GetTexture(textureFilePath)) {
			//std::cout << "This texture already exists!\n";
			return;
		}

		// Create a new texture
		std::shared_ptr<ResourceManager> rm = m_coord.GetSystem<ResourceManager>();

		// Get the number of cols and rows first, to determine whether the texture is a static texture or a textured spritesheet
		size_t firstUnderscore = textureFilePath.find_first_of('_');
		size_t secondUnderscore = textureFilePath.find_first_of('_', firstUnderscore + 1);
		size_t lastDot = textureFilePath.find_last_of('.');

		// Texture name provided no columns or rows, thus treat it as a static texture
		if (firstUnderscore == std::string::npos || secondUnderscore == std::string::npos || lastDot == std::string::npos || secondUnderscore > lastDot) {
			auto tex = std::make_unique<Texture>(textureFilePath, rm);
			//std::cout << "[Graphics] Created Static Texture: " << textureFilePath << '\n';
			AddTexture(textureFilePath, std::move(tex));
			return;
		}

		// Parse the number of columns and rows accordingly
		std::string numColStr = textureFilePath.substr(firstUnderscore + 1, secondUnderscore - firstUnderscore - 1);
		std::string numRowStr = textureFilePath.substr(secondUnderscore + 1, lastDot - secondUnderscore - 1);
		int numCol = std::stoi(numColStr);
		int numRow = std::stoi(numRowStr);

		//std::cout << "texture name is : " << textureFilePath << std::endl;

		auto tex = std::make_unique<Texture>(textureFilePath, rm, numCol, numRow);
		//std::cout << "[Graphics] Created Animated Texture: " << textureFilePath << '\n';
		AddTexture(textureFilePath, std::move(tex));

		// Dispatch a GraphicsEvent
		// To create the AnimationClip after creating a new texture
		if (m_dispatcher)
		{
			////std::cout << "[GRAPHICSYSTEM] dispatching event\n";
			m_dispatcher->QueueEvent(std::make_unique<GraphicsEvent>(GraphicsEvent::GraphicsAction::CreateTexture));
		}
	}

	void GraphicsSystem::UpdateTextureOfMaterial(const std::string& textureFilePath, Entity e) {
		auto& ren = m_coord.ModifyComponent<Renderable>(e);

		std::filesystem::path filePath = textureFilePath; 
		
		// Check whether the current texture exists
		Texture* tex = GetTexture(filePath.string());
		if (!tex) {
			// This is a new texture, add it to the resource map
			CreateTexture(filePath.string());

			// If it is a new texture, it means that there is no existing material for it
			// Create new material with this texture combination
			auto newMat = std::make_unique<Material>(GetShader("texture"), GetTexture(filePath.string()));
			std::string matName = filePath.filename().stem().string();
			// Name of the material will be the texture file path, but the name only
			AddMaterial(matName, std::move(newMat));


			// Change the name of this Renderable's material name to the new material name
			ren.materialName = matName;
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
				Texture* matTex = material->GetTexture();
				if (!matTex) continue;

				// If it exists already, change the name of this Renderable's material to the existing material name
				//std::cout << "Checking for texture name: " << textureFileName << std::endl;
				if (filePath.string() == matTex->GetTextureName()) {
					ren.materialName = filePath.filename().stem().string();
					found = true;
					//std::cout << "Material for this texture exists too!\n";
					break;
				}

			}
			if (!found) {
				//std::cout << "Material for this texture doesnt exist yet, creating one ...\n";
				// if it does not exist, create new material with this texture combination
				auto newMat = std::make_unique<Material>(GetShader("texture"), GetTexture(filePath.string()));
				std::string matName = filePath.filename().stem().string();
				// Name of the material will be the texture file path, but the name only
				AddMaterial(matName, std::move(newMat));
				// change the name of this Renderable's material name to the new material name
				ren.materialName = matName;
			}
			//std::cout << "Entity " << e << " material changed to " << ren.materialName << std::endl;
		}

		//std::cout << "UpdateTextureOfMaterial(), mat name: " << ren.materialName << std::endl;
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

	glm::mat4 GraphicsSystem::GetModelToWorldMatrix(Vec3 position, float rotation, Vec2 scale) {
		glm::mat4 model(1.f);
		model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
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

	void GraphicsSystem::ResolveRenderable(Entity e, Renderable& r) {
		CachedRenderData cache;
		cache.mesh = GetMesh(r.meshName);
		cache.material = GetMaterial(r.materialName);
		cache.valid = (cache.mesh && cache.material);
		m_renderCache[e] = cache;

		r.wasModified = false;
	}

	Entity GraphicsSystem::FindCamera() {
		// Camera Signature (only have components Camera and Tranform)
		Signature cameraSig;
		cameraSig.set(GetComponentTypeID<Camera>());
		cameraSig.set(GetComponentTypeID<Transform>());

		static bool inst = true;

		// Iterate through all living Entities in the COORDINATOR instead
		// Since the GraphicsSystem will not have a Camera Entity
		for (const Entity e : m_coord.GetLivingEntities()) {
			if ((m_coord.GetEntitySignature(e) & cameraSig) == cameraSig &&
				m_coord.GetComponent<Camera>(e).isMainCam) { // This camera Entity is the main camera
				inst = true;
				m_camera = e;
				return e;
			}
		}
		if (inst) {
			std::cerr << "Error: No main camera found in scene!\n";
			inst = false;
		}
		return INVALID_ENTITY;
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

	bool GraphicsSystem::ScreenSizeChanged() const noexcept {
		return (lastHvp != Hvp || lastWvp != Wvp);
	}

	void GraphicsSystem::TranslateUIEntities(Entity mainCam) {
		auto& camInfo = m_coord.GetComponent<Camera>(mainCam);

		// Only translate entities on the GAMEUI layer
		for (Layer& l : m_layers) {
			if (l.Empty() || !l.IsVisible() || l.Type() != LayerType::GAMEUI) { continue; }

			for (Entity e : l.Entities()) {
				// Reached the end of Valid Entities
				if (e == INVALID_ENTITY) { break; }

				// Obtain entity components
				auto& UIpos = m_coord.GetComponent<Transform>(e).UIbasePos;
				auto& UIscale = m_coord.GetComponent<Transform>(e).UIbaseScale;

				// Translate the position and scale of UI layer objects based on the current screen resolution
				float ratioPosX = UIpos.x / camInfo.width;
				float ratioPosY = UIpos.y / camInfo.height;
				float ratioScaleX = UIscale.x / camInfo.width;
				float ratioScaleY = UIscale.y / camInfo.height;

				// Get the new position and scaling based on the current screen resolution
				auto& trans = m_coord.ModifyComponent<Transform>(e);
				trans.pos.x = ratioPosX * Wvp;
				trans.pos.y = ratioPosY * Hvp;
				trans.scale.x = ratioScaleX * Wvp;
				trans.scale.y = ratioScaleY * Hvp;

				// Scale the collider too!!!
				if (m_coord.HasComponent<Collider>(e)) {
					auto& col = m_coord.ModifyComponent<Collider>(e);

					// Scale collider to match the scaled transform
					if (col.shape == ShapeType::AABB) {
						col.width = trans.scale.x;
						col.height = trans.scale.y;
					} else if (col.shape == ShapeType::Circle) {
						col.radius = std::max(trans.scale.x, trans.scale.y) * 0.5f;
					}
				}
			}
		}
	}

	const std::vector<std::string> GraphicsSystem::GetMaterialMapStrings() const {
		// Get all the existing materials keys stored in this system
		std::vector<std::string> materials{};
		for (const auto& [matName, mat] : m_materials) {
			materials.emplace_back(matName);
		}
		return materials;
	}

	void GraphicsSystem::DebugRenderableInformation() {

		// Materials
		std::cout << "===== MATERIALS IN STORE =====\n";
		for (auto& [matName, material] : m_materials) {
			std::cout << matName << std::endl;
		}

		// Textures
		std::cout << "===== TEXTURES IN STORE =====\n";
		for (auto& [texName, texture] : m_textures) {
			std::cout << texName << std::endl;
		}
	}

}	// NightSnack
