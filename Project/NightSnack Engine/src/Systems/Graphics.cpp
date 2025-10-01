#include "Graphics.h"
#include "MathLibrary/Math.h"

void GraphicsSystem::Init(Coordinator& coord) {
	// Default Shader;
	auto shader = std::make_unique<Shader>("assets/shaders/vertex_shader.vert", "assets/shaders/fragment_shader.frag");
	AddShader("default", std::move(shader));

	// Textures;
	auto bananaTex = std::make_unique<Texture>("assets/banana.jpg");
	AddTexture("banana", std::move(bananaTex));
	auto targetTex = std::make_unique<Texture>("assets/image_UI_target.png");
	AddTexture("target", std::move(targetTex));
	auto bgTex = std::make_unique<Texture>("assets/background.png");
	AddTexture("background", std::move(bgTex));

	// Meshes;
	AddMesh("point", Mesh::CreatePoint());
	AddMesh("line", Mesh::CreateLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f))); // Default facing upwards
	AddMesh("square", Mesh::CreateSquare());
	AddMesh("circle", Mesh::CreateCircle());
	AddMesh("AABBColliderMesh", Mesh::CreateAABBMesh());
	AddMesh("CircleColliderMesh", Mesh::CreateCircleMesh());

	// Materials with texture
	auto bananaMat = std::make_unique<Material>(GetShader("default"), GetTexture("banana"));
	bananaMat->SetBaseColor(1.f, 1.f, 1.f, 1.f);
	AddMaterial("bananaMat", std::move(bananaMat));
	auto targetMat = std::make_unique<Material>(GetShader("default"), GetTexture("target"));
	AddMaterial("targetMat", std::move(targetMat));
	auto backgroundMat = std::make_unique<Material>(GetShader("default"), GetTexture("background"));
	AddMaterial("backgroundMat", std::move(backgroundMat));

	// Materials with solid color
	auto colorMat = std::make_unique<Material>(GetShader("default"));
	colorMat->SetBaseColor(1.f, 1.f, 1.f, 1.f);
	AddMaterial("ColorMat", std::move(colorMat));

	// Dispatcher lambda
	m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event)
	{
		// This input event currently only handles key inputs for
		// - selection of an object (W)
		// - scaling of an object (A / D)
		// - rotating of an object (R)
		if (m_entities.empty())
			return;

		if (event.pressed == InputEvent::InputActionState::Pressed) {
			switch (event.inputAction) {
			case InputEvent::InputAction::SelectNext:
			{
				auto it = m_entities.begin();
				// Clear the previous selection
				if (m_selectedEntityIndex >= 0 && m_selectedEntityIndex < static_cast<int>(m_entities.size())) {
					std::advance(it, m_selectedEntityIndex); // Advance the iterator by n elements
					auto& ren = coord.ModifyComponent<Renderable>(*it);
					ren.isSelected = false;
				}

				// Move on to the next selection
				if (++m_selectedEntityIndex == static_cast<int>(m_entities.size())) {
					// Reset to selecting no objects once it reached the end
					m_selectedEntityIndex = -1;
					return;
				}

				// Set the current object selection to be true
				it = m_entities.begin(); // Reset it back to the start, before advancing again
				std::advance(it, m_selectedEntityIndex); // Advance the iterator by n elements
				auto& ren = coord.ModifyComponent<Renderable>(*it);
				ren.isSelected = true;
			}
			break;
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
			}
		}
	});
}

void GraphicsSystem::Update(float dt, Coordinator& coord) {
	(void)dt; // dt is unused for now...

	// Update viewport dimensions;
	Hvp = static_cast<float>(coord.GetWindowHeight());
	Wvp = static_cast<float>(coord.GetWindowWidth());

	// Camera stuff...
	Entity mainCam = FindCamera();
	if (mainCam == INVALID_ENTITY) return;

	// Get camera components
	auto& camPos = coord.GetComponent<Position>(mainCam);
	auto& camInfo = coord.GetComponent<Camera>(mainCam);

	glm::mat4 proj = GetCameraProjectionMatrix(camInfo.width, camInfo.height);
	glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(-camPos.x, -camPos.y, -camPos.z));

	for (Entity e : m_entities) {
		// Obtain entity components;
		auto& ren = coord.GetComponent<Renderable>(e);
		auto& pos = coord.GetComponent<Position>(e);
		auto& rot = coord.GetComponent<Rotation>(e);
		auto& scl = coord.GetComponent<Scale>(e);

		// @TODO: Event-Driven check whether e's renderable was changed...
		// if (renderable was changed) { ResolveRenderable(e, renderable); }

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

		glm::mat4 model = GetModelToWorldMatrix(pos, rot, scl, ren.renderLayer);
		glm::mat4 mvp = proj * view * model;

		Shader* shader = data.material->GetShader();
		Mesh* mesh = data.mesh;
		shader->Use();
		shader->SetMat4("uMVP", mvp);
		shader->SetBool("uIsSelected", ren.isSelected);

		if (m_coord.HasComponent<Color>(e)) {
			auto& col = m_coord.GetComponent<Color>(e);
			shader->SetVec3("uEntityColor", { col.r, col.g, col.b });
		}

		data.material->ApplyUniforms(ren.isWireframe, ren.isSelected);

		//GLint loc = glGetUniformLocation(data.material->GetShader()->GetID(), "uMVP");
		//std::cout << "uMVP loc=" << loc << "\n";

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

		// Debug Drawing, for entities with no texture
		if (m_enableDebugDraw) {
			// Drawing of colliders 
			if (m_coord.HasComponent<Collider>(e)) {
				Collider const& col = m_coord.GetComponent<Collider>(e);

				switch (col.shape) {
				case ShapeType::AABB:
				{
					// Model matrix according to its collider's scale
					glm::mat4 debugmodel = GetModelToWorldMatrix(pos, rot, Scale{ col.width, col.height }, RenderLayer::DEBUG);
					glm::mat4 debugmvp = proj * view * debugmodel;
					shader->SetMat4("uMVP", debugmvp);
					shader->SetVec3("uEntityColor", glm::vec3(0.f, 1.f, 0.f)); // Green color
					shader->SetVec3("uBaseColor", glm::vec3(0.f, 1.f, 0.f));
					GetMesh("AABBColliderMesh")->Draw();
				}
					break;

				case ShapeType::Circle:
				{
					// Model matrix according to its collider's scale
					glm::mat4 debugmodel = GetModelToWorldMatrix(pos, rot, Scale{ col.radius, col.radius }, RenderLayer::DEBUG);
					glm::mat4 debugmvp = proj * view * debugmodel;
					shader->SetMat4("uMVP", debugmvp);
					shader->SetVec3("uEntityColor", glm::vec3(0.f, 1.f, 0.f)); // Green color
					shader->SetVec3("uBaseColor", glm::vec3(0.f, 1.f, 0.f));
					GetMesh("CircleColliderMesh")->Draw();
				}
					break;

				}
			}

			// Drawing of velocity direction for moving objects
			if (m_coord.HasComponent<Velocity>(e)) {
				Velocity const& vel = m_coord.GetComponent<Velocity>(e);

				glm::vec3 velocity(vel.dx, vel.dy, 0.f);
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
					glm::mat4 debugModel = GetModelToWorldMatrix(pos, Rotation{ angle }, Scale{ 1.f, length }, RenderLayer::DEBUG);
					glm::mat4 debugMvp = proj * view * debugModel;
					
					shader->SetMat4("uMVP", debugMvp);
					shader->SetVec3("uEntityColor", glm::vec3(0.1f, 0.9f, 0.9f));
					shader->SetVec3("uBaseColor", glm::vec3(0.1f, 0.9f, 0.9f)); 
					// Points upwards by default, will be rotated accordingly
					GetMesh("line")->Draw();
				}
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------
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

// Compute the projection matrix ------------------------------------------------------------------
// Transforms camera coords to clip coords --------------------------------------------------------
glm::mat4 GraphicsSystem::GetCameraProjectionMatrix(float width, float height) {
	float halfWidth = width * 0.5f;
	float halfHeight = height * 0.5f;
	// Orthographic projection keeps objects the same size regardless of depth
	// Arguments in the order of (left, right, bottom, top, near, far)
	return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, Z_EPSILON, 10.f);
}

// Compute the ModelToWorld matrix ----------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
glm::mat4 GraphicsSystem::GetModelToWorldMatrix(Position position, Rotation rotation, Scale scale, RenderLayer layer) {
	glm::mat4 model(1.f);

	// Base z offset from the RenderLayer
	float layerZ = GetRenderLayerZPos(layer);
	// Final z offset of this renderable
	float z = layerZ + position.z;

	model = glm::translate(model, glm::vec3(position.x, position.y, z));
	model = glm::rotate(model, glm::radians(rotation.degrees), glm::vec3(0.f, 0.f, 1.f)); // Rotate about z-axis
	model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.f));
	return model;
}

// ------------------------------------------------------------------------------------------------
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

// ------------------------------------------------------------------------------------------------
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
	cameraSig.set(GetComponentTypeID<Position>());
	cameraSig.set(GetComponentTypeID<Rotation>());
	cameraSig.set(GetComponentTypeID<Scale>());

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

// Get the base z offset of the Renderable, based on the RenderLayer ------------------------------
// ------------------------------------------------------------------------------------------------
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

// Modify the scale of the currently selected object ----------------------------------------------
// ------------------------------------------------------------------------------------------------
void GraphicsSystem::ModifyEntityScale(Coordinator& coord, float value) {
	// No object is currently selected
	//if (m_selectedEntityIndex < 0)
	//	return;

	auto it = m_entities.begin();
	//std::advance(it, m_selectedEntityIndex); // Advance the iterator by n elements

	// Currently hardcoded to only 'control entity'
	std::advance(it, 3); 
	auto& ren = coord.GetComponent<Renderable>(*it);
	if (ren.materialName != "targetMat")
		return;

	auto& scale = coord.ModifyComponent<Scale>(*it);
	scale.x *= value;
	scale.y *= value;
	//auto& collider = coord.ModifyComponent<Collider>(*it);
	//collider.width *= value;
	//collider.height *= value;
	//collider.radius *= value;
}

// Modify the rotation of the currently selected object -------------------------------------------
// ------------------------------------------------------------------------------------------------
void GraphicsSystem::ModifyEntityRotation(Coordinator& coord, float value) {
	// No object is currently selected
	//if (m_selectedEntityIndex < 0)
	//	return;

	auto it = m_entities.begin();
	//std::advance(it, m_selectedEntityIndex); // Advance the iterator by n elements

	// Currently hardcoded to only 'control entity'
	std::advance(it, 3); 
	auto& ren = coord.GetComponent<Renderable>(*it);
	if (ren.materialName != "targetMat")
		return;

	auto& rotation = coord.ModifyComponent<Rotation>(*it);
	rotation.degrees += value;
	if (rotation.degrees >= 360.f)
		rotation.degrees -= 360.f;
}

Position GraphicsSystem::ScreenToWorld(Position const& screenPos) {
	Position pos{};
	// To convert from screen to clip: X -> -Wvp/2 + x; Y -> Hvp/2 - y; 
	pos.x = (-Wvp / 2.f) + screenPos.x;
	pos.y = (Hvp / 2.f) - screenPos.y;
	
	// Clip to World: take into account CameraPos;
	Entity cam = FindCamera();

	pos.x += m_coord.GetComponent<Position>(cam).x;
	pos.y += m_coord.GetComponent<Position>(cam).y;

	return pos;
}

void GraphicsSystem::RemoveCache(Entity e) {
	m_renderCache.erase(e);
}
