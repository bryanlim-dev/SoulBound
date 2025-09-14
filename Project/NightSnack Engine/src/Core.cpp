#include "Core.h"
#include "Systems/Movement.h"
#include "ECS/Component.h"
#include "ECS/ComponentManager.h"
#include "ECS/EntityManager.h"
#include "Systems/ISystem.h"


// ------------------------------------------------------------------------------------------------
// CORE
// ------------------------------------------------------------------------------------------------

void CoreEngine::Initialize() {
	// Initialize ECS Coordinator and Managers;
	Coord.Initialize();

	// Register all Components;
	Coord.RegisterComponent<Position>();
	Coord.RegisterComponent<Velocity>();

	// Add more Components here...

	// Register all Systems;
	Coord.RegisterSystem<MovementSystem>();
	Signature Sig{};								// Create a Signature for System to know what Components it needs;
	Sig.set(GetComponentTypeID<Position>());	// Set Position to true in signature;
	Sig.set(GetComponentTypeID<Velocity>());	// Set Velocity to true in signature;
	Coord.SetSystemSignature<MovementSystem>(Sig);	// Set actual System Signature;
	
	// Add more Systems here...
}

void CoreEngine::Terminate() {
	// Empty for now...
}

void CoreEngine::Update(double deltaTime) {
	// Iterate through Systems and Update them through Coordinator;
	for (auto& System : Systems) {
		System->Update(deltaTime, Coord);
	}
}

template <typename T>
void CoreEngine::AddSystem(std::shared_ptr<T> System) {
	// Assert that only objects derived from ISystem can be used;
	static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");
	// Insert into vector of Systems;
	Systems.push_back(std::static_pointer_cast<ISystem>(system));
}

template <typename T>
void CoreEngine::RemoveSystem(std::shared_ptr<T> System) {
	// Shift unwanted Systems to the end of the vector, then uses erase() to remove them;
	Systems.erase(std::remove(Systems.begin(), Systems.end(), system), Systems.end());
}
