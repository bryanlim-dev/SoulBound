#include "EntityFactory.h"


void EntityFactory::Update(float dt, Coordinator& coord) {

	static float time{};
	time += dt;

	if (time > 1.f)
		if (m_coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::W)) {
			Transform trans{};
			trans.pos.x = m_coord.GetSystem<InputSystem>()->GetMousePosX();
			trans.pos.y = m_coord.GetSystem<InputSystem>()->GetMousePosY();

			trans.scale.x = trans.scale.y = 100.f;

			Mesh circle = Mesh::CreateCircle();
			// Load shader
			Shader shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
			Material blue(&shader, glm::vec3(0.f, 0.f, 1.f));
			Renderable r1{ &circle, &blue, false };

			Entity e = Create("BlueCircle", trans, r1);
			std::cout << "Created Entity: " << e << "\n";
		}
}

Entity EntityFactory::Create(std::string const& name) {
	Entity e = m_prefabManager.CreateFromPrefab(name, m_archetypes);
	return e;
}

Entity EntityFactory::Create(std::string const& name, Transform trans, Renderable r1) {
	Entity e = m_prefabManager.CreateFromPrefab(name, m_archetypes);

	m_coord.GetComponent<Position>(e) = trans.pos;
	m_coord.GetComponent<Rotation>(e) = trans.rot;
	m_coord.GetComponent<Scale>(e) = trans.scale;

	m_coord.GetComponent<Renderable>(e) = r1;

	return e;
}