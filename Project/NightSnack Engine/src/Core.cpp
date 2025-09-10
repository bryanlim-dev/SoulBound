#include "Core.h"

void CoreEngine::Initialize() {

	// Initialize all systems by calling member functions;
	for (unsigned i = 0; i < Systems.size(); ++i)
		Systems[i]->Initialize();

}

void CoreEngine::Terminate() {

	// Terminate all systems by calling member functions;
	for (unsigned i = 0; i < Systems.size(); ++i)
		Systems[i]->Terminate();

}

void CoreEngine::Update(float deltaTime) {

	// Update all systems by calling member functions;
	for (unsigned i = 0; i < Systems.size(); ++i)
		Systems[i]->Update(deltaTime);

}

void CoreEngine::AddSystem(System* system) {

	// Add the system to the vector of systems;
	Systems.push_back(system);

}

void CoreEngine::RemoveSystem(System* system) {

	// Move the system to be removed to the end of the vector and erase it;
	Systems.erase(std::remove(Systems.begin(), Systems.end(), system), Systems.end());

}