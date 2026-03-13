/******************************************************************************
 * File:        CharacterSystem.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%)
 *
 * Description:
 * This file defines the the class CharacterSystem and its functions, it
 * is a system that tracks and store CharacterProperties on entities and updates them as needed
 * 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Component.h"

#pragma once
namespace NightSnack {
	class CharacterSystem : public ISystem {
	public:

		/************************************************************************/
		/*!
		\brief
			Constructor for CharacterSystem

		\param coord
			Coordinator reference to access components and entities

		\param disp
			EventDispatcher reference to subscribe to events
		*/
		/************************************************************************/
		CharacterSystem(Coordinator& coord, EventDispatcher& disp) : m_coord(coord), m_dispatcher(&disp) {
			m_systemSignature.set(GetComponentTypeID<Character>());
		}

		/************************************************************************/
		/*!
		\brief
			Updates the CharacterSystem.

		\param coord
			Coordinator reference to access components and entities

		\param disp
			EventDispatcher reference to subscribe to events
		*/
		/************************************************************************/
		void Update() override;

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const { return "CharacterSystem"; }
		
		/************************************************************************/
		/*!
		\brief
			Updates the entity property map to reflect the current 
			CharacterProperties of all entities.
		*/
		/************************************************************************/
		void UpdateEntityPropertyMap();

		/************************************************************************/
		/*!
		\brief
			Finds entities with the specified CharacterProperties.

		\param propertities
			The CharacterProperties to search for.
		*/
		/************************************************************************/
		std::list<Entity> FindEntityWithPropertities(CharacterProperties propertities);

		/************************************************************************/
		/*!
		\brief
			Converts a CharacterProperties enum value to its 
			corresponding string representation.

		\param prop
			The CharacterProperties enum value to convert.
		*/
		/************************************************************************/
		inline static std::string CharacterPropertiesToString(CharacterProperties prop)
		{
			switch (prop)
			{
#define PROPERTY(name) case name: return std::string(#name);
				CHARACTER_PROPERTIES_LIST
#undef PROPERTY
			default:
				return "UNKNOWN";
			}
		}

		/************************************************************************/
		/*!
		\brief
			Converts a CharacterProperties enum value to its 
			corresponding string representation.

		\param prop
			The CharacterProperties enum value to convert.
		*/
		/************************************************************************/
		inline static bool StringToCharacterProperties(
			std::string_view str,
			CharacterProperties& out)
		{
#define PROPERTY(name) if (str == #name) { out = name; return true; }
			CHARACTER_PROPERTIES_LIST
#undef PROPERTY

				return false;
		}

	private:
		Signature m_systemSignature{};
		Coordinator& m_coord;
		EventDispatcher* m_dispatcher{ nullptr };


		//Each property has a list
		//Max size of arrray is max propertities
		std::list<Entity> m_entityPropertyArr[MAX_PROPERTIES];
	};
}