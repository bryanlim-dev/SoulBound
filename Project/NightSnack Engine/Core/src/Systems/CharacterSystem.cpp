#include "CharacterSystem.h"

namespace NightSnack {



	void CharacterSystem::Update()
	{
		UpdateEntityPropertyMap();
	}

	void CharacterSystem::UpdateEntityPropertyMap()
	{

		for (int prop = static_cast<int>(MAX_PROPERTIES); prop < static_cast<int>(MAX_PROPERTIES); prop++)
		{	
			m_entityPropertyArr[prop].clear();
		}

		for (Layer& l : m_layers) {
			if (l.Empty() || !l.IsVisible() || !l.IsActive()) { continue; }
			for (Entity e : l.Entities()) {

				if (e == INVALID_ENTITY) { break; }
				if (m_coord.HasComponent<Character>(e))
				{
					std::vector<CharacterProperties>const & props = m_coord.ModifyComponent<Character>(e).m_properties;
					for (CharacterProperties const& property : props)
					{
						m_entityPropertyArr[property].push_back(e);
					}
				}

			}
		}
	}

	std::list<Entity> CharacterSystem::FindEntityWithPropertities(CharacterProperties propertities)
	{
		if (propertities < MAX_PROPERTIES && propertities > START_PROPERTIES)
			return m_entityPropertyArr[propertities];

		return std::list<Entity>();
	}
}