// SERIALIZER_HPP

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "Core/ECS/Component.h"
#include "Core/ECS/Entity.h"

namespace NightSnack {

	// COMPONENT SERIALIZATION
	// --------------------------------------------------------------

	// Transform
	void to_json(json& j, Transform const& t);
	void from_json(json const& j, Transform& t);

	// RigidBody
	void to_json(json&, RigidBody const&);
	void from_json(json const&, RigidBody&);

	// Script
	void to_json(json&, Script const&);
	void from_json(json const&, Script&);

	// Collider
	void to_json(json&, Collider const&);
	void from_json(json const&, Collider&);

}	// Namespace NightSnack::Serializer

#endif	// SERIALIZER_HPP