#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Component.h"

#pragma once
namespace NightSnack {
	class CharacterSystem : public ISystem {
	public:
		CharacterSystem(Coordinator& coord, EventDispatcher& disp) : m_coord(coord), m_dispatcher(&disp) {
			m_systemSignature.set(GetComponentTypeID<Character>());
		}

		void Update() override;

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const { return "CharacterSystem"; }

		void UpdateEntityPropertyMap();

		std::list<Entity> FindEntityWithPropertities(CharacterProperties propertities);

		//Returns if CharacterProperties can be turned into string
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

		//Returns if CharacterProperties enum can be turned into a string
		//out returns the property
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