#include "Core/ECS/Coordinator.h"


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

		std::list<Entity> FindEntityWithPropertities(CharacterPropertities propertities);

	private:
		Signature m_systemSignature{};
		Coordinator& m_coord;
		EventDispatcher* m_dispatcher{ nullptr };


		//Each property has a list
		//Max size of arrray is max propertities
		std::list<Entity> m_entityPropertyArr[MAX_PROPERTITIES];
	};
}