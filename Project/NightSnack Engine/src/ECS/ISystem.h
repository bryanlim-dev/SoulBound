#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include <set>				// ISystem -> Entities
#include <unordered_map>	// SystemManger -> Signatures, Systems
#include <memory>

#include "ECS/Entity.h"

class Coordinator;

class ISystem {

public:
	virtual ~ISystem() = default;
	
	// Entities that the system can access;
	std::set<Entity> Entities;
	// Every system must define an Update function;
	virtual void Update(double dt, Coordinator& Coord) = 0;
	// Every system must expose its signature;
	virtual Signature GetSignature() const = 0;

};

// Get Unique ID for System upon Registering System;
// Inline is to prevent multiple definition error, as this function is defined
// in a header file;
inline SystemID GetUniqueSystemID() {

	// Static counter that lasts for the entire program lifetime;
	static SystemID lastID = 0u;
	// Return ID then increment for next call;
	return lastID++;

}

// Obtain SystemID for a specific System of type T;
template <typename T>
inline SystemID GetSystemID() noexcept {

	// static: typeID is only created once, so T System will maintain the same ID
	// for the program lifetime;
	static SystemID typeID = GetUniqueSystemID();
	return typeID;
}

class SystemManager {

public:
	template <typename T>
	std::shared_ptr<T> RegisterSystem();

	template <typename T>
	void SetSignature(Signature sig);

	void EntityDestroyed(Entity e);

	void EntitySignatureChanged(Entity e, Signature EntitySig);

	template <typename T>
	Signature GetSystemSignature();
	template <typename T>
	std::shared_ptr<T> GetSystem();

	std::unordered_map<SystemID, Signature> Signatures;
	std::unordered_map<SystemID, std::shared_ptr<ISystem>> Systems;

};


// Register a System of type T into the SystemManager;
template <typename T>
std::shared_ptr<T> SystemManager::RegisterSystem() {

	// Obtain type index for mapping;
	SystemID typeIdx = GetSystemID<T>();
	// Shared pointer of type System;
	auto system = std::make_shared<T>();
	// Insert system into Map of systems;
	Systems[typeIdx] = system;

	// SetUp System Signature in Signatures map;
	Signatures[typeIdx] = system->GetSignature();

	// Return a smart pointer to system registered;
	return system;

}

// Obtain a Signature using type_index and insert Signature into unordered Map of Signatures;
template <typename T>
void SystemManager::SetSignature(Signature sig) {

	// Obtain type index for mapping;
	SystemID typeIdx = GetSystemID<T>();
	// Now sig is linked with typeIdx and System<T> pointer stored in Systems unordered map;
	Signatures[typeIdx] = sig;

}

// If an Entity is destroy, this function's purpose is to remove the destroyed Entity
// from all Systems;
inline void SystemManager::EntityDestroyed(Entity e) {

	// Iterate through unordered Map of Systems, remove Entity e from set of 
	// Entities that the Systems can access, so they do not work on destroyed Entities;
	for (auto& pair : Systems) {
		pair.second->Entities.erase(e);
	}

}

// In the event of an Entity's System Signature being changed, update every
// Systems' set of Entities, adding and removing where necessary;
inline void SystemManager::EntitySignatureChanged(Entity e, Signature EntitySig) {

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

template <typename T>
Signature SystemManager::GetSystemSignature() {

	// Obtain type index for mapping;
	SystemID typeIdx = GetSystemID<T>();
	// Obtain Signature of System of type T in map;
	return Signatures[typeIdx];
}

template <typename T>
std::shared_ptr<T> SystemManager::GetSystem() {

	// Obtain type index for mapping;
	SystemID typeIdx = GetSystemID<T>();
	// Find pointer to desired System in map;
	return std::static_pointer_cast<T>(Systems[typeIdx]);
}

#endif // ISYSTEM_HPP