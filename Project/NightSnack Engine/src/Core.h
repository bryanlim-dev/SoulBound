#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include "Systems/System.h"

class CoreEngine {

public:
	void Initialize();
	void Terminate();

	void Update(float deltaTime);

	void AddSystem(System* system);
	void RemoveSystem(System* system);

private:
	std::vector<System*> Systems;
};

#endif // CORE_HPP
