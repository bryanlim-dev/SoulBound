#include "Systems/System.h"

// Register a System of type T into the SystemManager;
template <typename T>
std::shared_ptr<T> SystemManager::RegisterSystem() {

	// Obtain type index for mapping;
	std::type_index typeIdx(typeid(T));
	// Shared pointer of type System;
	auto system = std::make_shared<T>();
	// Insert system into Map of systems;
	Systems[typeIdx] = system;

	// Return a smart pointer to system registered;
	return system;

}

// Obtain a Signature using type_index and insert Signature into unordered Map of Signatures;
template <typename T>
void SystemManager::SetSignature(Signature sig) {

	// Obtain type index for mapping;
	std::type_index typeIdx(typeid(T));
	// Now sig is linked with typeIdx and System<T> pointer stored in Systems unordered map;
	Signatures[typeIdx] = sig;

}

// If an Entity is destroy, this function's purpose is to remove the destroyed Entity
// from all Systems;
void SystemManager::EntityDestroyed(Entity e) {

	// Iterate through unordered Map of Systems, remove Entity e from set of 
	// Entities that the Systems can access, so they do not work on destroyed Entities;
	for (auto& pair : Systems) {
		pair.second->Entities.erase(e);
	}

}

// In the event of an Entity's System Signature being changed, update every
// Systems' set of Entities, adding and removing where necessary;
void SystemManager::EntitySignatureChanged(Entity e, Signature EntitySig) {

	// Iterate through unordered Map of Systems;
	for (auto const& pair : Systems) {
		// Signature of System in current iteration;
		auto const& SystemSig = Signatures[pair.first];
		// Pointer to System stored in unordered map, of current iteration;
		auto System = pair.second;

		// If Entity contains all components matching with current System;
		if ((EntitySig & SystemSig) == SystemSig) {
			// Insert Entity into System set of Entities;
			System->Entities.insert(e);
		}
		else {
			// Else remove Entity from set of Entities;
			System->Entities.erase(e);
		}
	}

}