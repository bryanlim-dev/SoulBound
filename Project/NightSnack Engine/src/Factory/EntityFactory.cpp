#include "EntityFactory.h"

Entity EntityFactory::Create() {
	Entity e = m_coord.CreateEntity();
	
	return e;
}