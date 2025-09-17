#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include <string>
#include <fstream>
#include <unordered_map>

#include "./ECS/Coordinator.h"

struct Archetype {
	std::unordered_map<std::string, std::vector<double>> components;
};

class EntityFactory {

public:
	// Factory cannot be default constructed;
	EntityFactory() = delete;

	// Factory needs to obtain a reference to the Coordinator and a map of archetypes (parsed from a file by resource manager);
	EntityFactory(Coordinator& coord, std::unordered_map<std::vector<std::string>, Archetype>&& archetypes)
		: m_coord(coord), m_archetypes(std::move(archetypes)) {}

	Entity Create(std::string const& type);

private:
  Coordinator& m_coord;
	std::unordered_map<std::vector<std::string>, Archetype> m_archetypes;
};

#endif // ENTITYFACTORY_H