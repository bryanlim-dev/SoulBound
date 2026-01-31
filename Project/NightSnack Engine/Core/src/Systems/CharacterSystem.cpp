#include "CharacterSystem.h"

namespace NightSnack {



	void CharacterSystem::Update()
	{
		UpdateEntityPropertyMap();
	}

	void CharacterSystem::UpdateEntityPropertyMap()
	{

		for (int prop = static_cast<int>(START_PROPERTITIES); prop < static_cast<int>(MAX_PROPERTITIES); prop++)
		{	
			m_entityPropertyArr[prop].clear();
		}

		for (Layer& l : m_layers) {
			if (l.Empty() || !l.IsVisible() || !l.IsActive()) { continue; }
			for (Entity e : l.Entities()) {

				if (e == INVALID_ENTITY) { break; }
				if (m_coord.HasComponent<Character>(e))
				{
					std::list<CharacterPropertities>const & props = m_coord.ModifyComponent<Character>(e).m_propertities;
					for (CharacterPropertities const& property : props)
					{
						m_entityPropertyArr[property].push_back(e);
					}
				}

			}
		}
	}

	std::list<Entity> CharacterSystem::FindEntityWithPropertities(CharacterPropertities propertities)
	{
		if (propertities < MAX_PROPERTITIES && propertities > START_PROPERTITIES)
			return m_entityPropertyArr[propertities];

		return std::list<Entity>();
	}
}