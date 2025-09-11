#ifndef COORD_HPP
#define COORD_HPP

#include "ECS/Types.h"

// Coordinator manages the ECS Managers;
// Provides an interface that can access all of ECS Managers' functions and data;
class Coordinator {

public:
	void Initialize();
	//void Update(double deltaTime);
	
	// ECS Interface;
	Entity CreateEntity();
	void DestroyEntity(Entity e);

	template <typename T>
	void RegisterComponent();

	template <typename T>
	void AddComponent(Entity e, T component);
	template <typename T>
	void RemoveComponent(Entity e);

	template <typename T>
	T& GetComponent(Entity e);
	template <typename T>
	ComponentType GetComponentType();

	template<typename T>
	std::shared_ptr<T> RegisterSystem();

	template<typename T>
	void SetSystemSignature(Signature sig);

private:
	// ECS Managers;
	std::unique_ptr<EntityManager> Entity_M;
	std::unique_ptr<ComponentManager> Component_M;
	std::unique_ptr<SystemManager> System_M;
};

#endif // COORD_HPP