/******************************************************************************
 * File:        Serializer.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 60%)
 *				Bryan Lim Jun Jie (Secondary, 15%) - Audio and button info components
 *				Goh Bei Lai (Secondary, 25%) - Font, Particle and Collectible components
 *
 * Description:
 * This file declares functions to serialize and deserialize components to and from json format.
 * Each component has a to_json and from_json function that converts the component to json and vice versa.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

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

	// Particle
	void from_json(json const& j, Particle& p);

	// Collectible
	void to_json(json& j, Collectible const& c);
	void from_json(json const& j, Collectible& c);

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