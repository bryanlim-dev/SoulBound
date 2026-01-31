// SERIALIZER_HPP

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "Core/ECS/Component.h"
#include "Core/ECS/Entity.h"

#if 1

namespace NightSnack {

	void to_json(json& j, Vec3 const& v);
	void from_json(json const& j, Vec3& v);

	void to_json(json& j, Vec2 const& v);
	void from_json(json const& j, Vec2& v);

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

	// Renderable
	void to_json(json&, Renderable const&);
	void from_json(json const&, Renderable&);

	// Color
	void to_json(json&, Color const&);
	void from_json(json const&, Color&);

	// Audio
	void to_json(json&, Audio const&);
	void from_json(json const&, Audio&);

	// Camera
	void to_json(json&, Camera const&);
	void from_json(json const&, Camera&);

	// PlayerController
	void to_json(json&, PlayerController const&);
	void from_json(json const&, PlayerController&);

	//// States
	//void to_json(json&, States const&);
	//void from_json(json const&, States&);

	// Hierarchy
	void to_json(json&, Hierarchy const&);
	void from_json(json const&, Hierarchy&);

	// Animator
	void to_json(json&, Animator const&);
	void from_json(json const&, Animator&);

	// Font
	void to_json(json&, Font const&);
	void from_json(json const&, Font&);

	// ButtonInfo
	void to_json(json&, ButtonInfo const&);
	void from_json(json const&, ButtonInfo&);

	// Sprite
	void to_json(json&, Sprite const&);
	void from_json(json const&, Sprite&);

	// Character
	void to_json(json&, Character const&);
	void from_json(json const&, Character&);

}	// Namespace NightSnack::Serializer

#endif

#endif	// SERIALIZER_HPP