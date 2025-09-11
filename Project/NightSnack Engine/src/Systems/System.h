#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "Systems/ISystem.h"
#include "Core.h"

class SystemManager {

public:
	template <typename T>
	std::shared_ptr<T> RegisterSystem();

	template <typename T>
	void SetSignature(Signature sig);

	void EntityDestroyed(Entity e);

	void EntitySignatureChanged(Entity e, Signature EntitySig);

	std::unordered_map<std::type_index, Signature> Signatures;
	std::unordered_map<std::type_index, std::shared_ptr<ISystem>> Systems;

};

#endif // SYSTEM_HPP

