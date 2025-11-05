/******************************************************************************
 * File:        Entity.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Const expressions and aliases for types relating to the ECS core.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cstdint>
#include <bitset>

namespace NightSnack {

	// Maximum number of entities in the system;
	constexpr size_t MAX_ENTITIES = 3000;
	// Maximum number of components;
	constexpr size_t MAX_COMPONENTS = 32;

	// Unique IDs;
	using Entity = std::uint32_t;
	using SystemID = std::uint8_t;
	using ComponentType = std::uint8_t;

	// Signatures for Systems to recognize which Entities to work on;
	// It is a set of bits from 0 to MAX_COMPONENTS,
	// Whenever a new component is registered, it will have an ID from 0 to MAX_COMPONENTS;
	// Entity Manager, Coordinator and System Manager will each maintain and compare Signatures;
	using Signature = std::bitset<MAX_COMPONENTS>;

	constexpr Entity INVALID_ENTITY = UINT32_MAX;

}	// NightSnack

#endif // ENTITY_HPP
