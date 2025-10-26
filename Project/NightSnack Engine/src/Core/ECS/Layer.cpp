#include "Layer.h"

namespace NightSnack {

	// LAYER -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	void Layer::Insert(Entity e) {

		// Only unique entries;
		if (std::find(m_entities.begin(), m_entities.end(), e) != m_entities.end()) {
			std::cerr << "Entity " << e << " is already in layer\n";
			return;
		}

		// Mapping Entity ID to Index in Layer -> Last current index;
		m_entityToIndex[e] = m_size;
		m_indexToEntity[m_size] = e;

		// Insert Entity to end of Layer's Array;
		m_entities[m_size] = e;

		// Update size;
		++m_size;
	}

	void Layer::Remove(Entity e) {
		std::unordered_map<Entity, size_t>::iterator it = m_entityToIndex.find(e);
		if (it == m_entityToIndex.end()) {
			std::cout << "Error: Tried to remove non-existent entity from Layer: " << m_id << '\n';
			return;
		}
		if (m_size <= 0) {
			std::cout << "Error: Tried to remove an entity from empty Layer: " << m_id << '\n';
			return;
		}

		for (size_t i{ it->second }; i + 1 < m_size; ++i) {
			// Shift all entities after Removed one space forward;
			m_entities[i] = m_entities[i + 1];

			// Update Mappings;
			m_entityToIndex[m_entities[i]] = i;
			m_indexToEntity[i] = m_entities[i];
		}
		// Invalidate out-of-scope entity;
		m_entities[m_size - 1] = INVALID_ENTITY;

		// Erase mappings for removed entity;
		m_entityToIndex.erase(m_indexToEntity[m_size - 1]);
		m_indexToEntity.erase(m_size - 1);
		// Update size;
		--m_size;
	}

	bool Layer::Has(Entity e) const noexcept {
		if (std::find(m_entities.begin(), m_entities.end(), e) != m_entities.end()) {
			return true;
		}
		return false;
	}

	bool Layer::operator<(Layer const& rhs) {

		return m_type < rhs.m_type;
	}

}
