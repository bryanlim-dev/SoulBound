#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <unordered_multimap>
#include <string>
#include <json.hpp>
#include <fstream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "Factory/PrefabManager.h"
#include "Systems/Input.h"

using json = nlohmann::json;

class EntityFactory : public ISystem {

public:

	EntityFactory(Coordinator& coord, std::string const& archetypes_filename)
		: m_coord(coord), m_prefabManager(coord) {

		m_prefabManager.Initialize();
		LoadJsonFile(archetypes_filename);
	}

	void LoadJsonFile(std::string const& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open archetypes file: " + filename);
		}

		try {
			file >> m_archetypes;
		}
		catch (json::parse_error& e) {
			throw std::runtime_error("JSON parse error at byte " + std::to_string(e.byte) + ": " + e.what());
		}
	}

	void Update(float dt, Coordinator& coord) override;

	Signature GetSignature() const override { return Signature{}; }
	std::string GetName() const override { return "EntityFactory"; }

	Entity Create(std::string const& name);
	Entity Create(std::string const& name, Transform trans);

	Entity Clone(Entity e, Transform trans);

private:
	Coordinator& m_coord;
	PrefabManager m_prefabManager;
	json m_archetypes{};;
};

// First obtains all archetypes through initialization;

// LoadLevel() -> SetComponents to create entities with specified transforms and components;



#endif // ENTITYFACTORY_H