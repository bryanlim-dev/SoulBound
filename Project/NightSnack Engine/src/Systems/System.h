#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "ECS/Types.h"

class System {

public:
	// Entities that the system can access;
	std::set<Entity> Entities;
	// Every system must define an Update function;
	virtual void Update(double dt) = 0;

};

class SystemManager {

public:
	template <typename T>
	std::shared_ptr<T> RegisterSystem();

	template <typename T>
	void SetSignature(Signature sig);

	void EntityDestroyed(Entity e);

	void EntitySignatureChanged(Entity e, Signature EntitySig);

private:
	std::unordered_map<std::type_index, Signature> Signatures;
	std::unordered_map<std::type_index, std::shared_ptr<System>> Systems;

};

#endif // SYSTEM_HPP

