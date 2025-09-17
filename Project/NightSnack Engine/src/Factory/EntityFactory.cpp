#include "EntityFactory.h"

Entity EntityFactory::Create(std::string const& type) {
	Entity e = m_coord.CreateEntity();
	
	return e;
}