#include "EntityFactory.h"

void EntityFactory::Update(float dt, Coordinator& coord) {

	static float time{};
	time += dt;

	if (time > 1.f) {
		if (m_coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::W)) {
			Transform trans{};
			trans.pos.x = coord.GetSystem<InputSystem>()->GetMousePosX();
			trans.pos.y = coord.GetSystem<InputSystem>()->GetMousePosY();

			trans.scale.x = trans.scale.y = 100.f;

			Entity e = Create("BlueCircle", trans);
			std::cout << "Created Entity: " << e << " at "
				<< trans.pos.x << ", " << trans.pos.y << "\n";

			time = 0.f;
		}
		if (m_coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::A)) {
			Transform trans{}; static int offset{ ((rand() % 1600) - 800) };
			trans.pos.x = offset += ((rand() % 1600) - 800);
			trans.pos.y = offset += ((rand() % 900) - 450);

			trans.scale.x = trans.scale.y = 50.f;

			Entity e = Create("RedSquare", trans);
			std::cout << "Created Entity: " << e << " at "
				<< trans.pos.x << ", " << trans.pos.y << "\n";

			time = 0.f;
		}
		if (m_coord.GetSystem<InputSystem>()->IsKeyPressed(InputSystem::S)) {
			Transform trans{}; static int offset{ ((rand() % 1600) - 800) };
			trans.pos.x = offset += ((rand() % 1600) - 800);
			trans.pos.y = offset += ((rand() % 900) - 450);

			trans.scale.x = trans.scale.y = 100.f;

			Entity e = Create("Player", trans);
			std::cout << "Created Entity: " << e << " at "
				<< trans.pos.x << ", " << trans.pos.y << "\n";

			time = 0.f;
		}

	}	
}

Entity EntityFactory::Create(std::string const& name) {
	Entity e = m_prefabManager.CreateFromPrefab(name, m_archetypes);
	return e;
}

Entity EntityFactory::Create(std::string const& name, Transform trans) {
	Entity e = m_prefabManager.CreateFromPrefab(name, m_archetypes);

	m_coord.ModifyComponent<Position>(e) = trans.pos;
	m_coord.ModifyComponent<Rotation>(e) = trans.rot;
	m_coord.ModifyComponent<Scale>(e) = trans.scale;

	//m_coord.GetComponent<Renderable>(e) = r1;

	return e;
}

Entity EntityFactory::Clone(Entity e, Transform trans) {
	//std::string name = entityToPrefab.find(e);

	Entity clone = m_prefabManager.CreateFromPrefab(name, m_archetypes);

	m_coord.ModifyComponent<Position>(clone) = trans.pos;
	m_coord.ModifyComponent<Rotation>(clone) = trans.rot;
	m_coord.ModifyComponent<Scale>(clone) = trans.scale;

	return clone;
}