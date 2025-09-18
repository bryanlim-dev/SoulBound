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
	// e.g: <"Position", static_pointer_cast<IComponent>(Position{ 1.f, 2.5f })>; components["Position"] = 1.f, 2.5f;
	std::unordered_map<std::string, IComponent> components;
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
	// e.g: <"Player", Archetype p1>; <- player archetype;
	std::unordered_map<std::string, Archetype> m_archetypes;
};

#endif // ENTITYFACTORY_H