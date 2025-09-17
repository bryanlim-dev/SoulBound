#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <string>
#include <any>
#include <unordered_map>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"

struct Archetype {
	ComponentType componentID;
	std::vector<std::any> Data;
};

class EntityFactory : public ISystem {

public:
	// Factory cannot be default constructed;
	EntityFactory() = delete;

	// Factory needs to obtain a reference to the Coordinator and a map of archetypes (parsed from a file by resource manager);
	EntityFactory(Coordinator& coord, std::unordered_map<std::vector<std::string>, Archetype> const& archetypes)
		: m_coord(coord){}

	Entity Create(std::string const& type);

private:
	Coordinator& m_coord;
	std::unordered_map<std::string, Archetype> m_archetypes;
};

#endif // ENTITYFACTORY_H