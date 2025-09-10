#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <set>
#include "ECS/Entity.h"

class System {

public:
	virtual void Initialize();
	virtual void Terminate();

	virtual void Update(float deltaTime) = 0;

private:
	// Entities that the system can access;
	std::set<Entity> entities;

};

class SystemManager {

public:


};

#endif // SYSTEM_HPP

