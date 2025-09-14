#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <memory>
#include "ECS/Coordinator.h"

class CoreEngine {

public:
	void Initialize();
	void Terminate();

	void Update(double dt);

	template <typename T>
	void AddSystem(std::shared_ptr<T> System);
	template <typename T>
	void RemoveSystem(std::shared_ptr<T> System);

private:
	std::vector<std::shared_ptr<ISystem>> Systems;
	Coordinator Coord;
};

#endif // CORE_HPP
