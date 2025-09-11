#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include "Systems/System.h"
#include "ECS/Component.h"
#include "ECS/Entity.h"
#include "ECS/Types.h"

class CoreEngine {

public:
	void Initialize();
	void Terminate();

	void Update(double dt);

	void AddSystem(System* system);
	void RemoveSystem(System* system);

	// ECS Interface;
	Entity CreateEntity();
	void DestroyEntity(Entity e);

	template <typename T>
	void RegisterComponent();

	template <typename T>
	void AddComponent(Entity e, T component);

	template <typename T>
	T& GetComponent(Entity e);

	template<typename T>
	std::shared_ptr<T> RegisterSystem();

	template<typename T>
	void SetSystemSignature(Signature sig);

private:
	std::vector<System*> Systems;
	ComponentManager Component_M;;
	EntityManager Entity_M;
	SystemManager System_M;
};

#endif // CORE_HPP
