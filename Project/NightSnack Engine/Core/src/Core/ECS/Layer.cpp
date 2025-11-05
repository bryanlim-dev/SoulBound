#include "Layer.h"

namespace NightSnack {

	// LAYER -----------------------------------------------------------------------------------------
	// -----------------------------------------------------------------------------------------------
	void Layer::Insert(Entity e) {

		// Only unique entries;
		if (m_entityToIndex.find(e) != m_entityToIndex.end()) {
			//std::cerr << "Entity " << e << " is already in layer " << m_id << '\n';
			return;
		}
		// Check capacity (assuming m_entities is a vector or fixed array)
		if (m_size >= m_entities.size()) {
			std::cout << "Error: Layer " << m_id << " is full\n";
			return;
		}
		// Mapping Entity ID to Index in Layer -> Last current index;
		m_entityToIndex[e] = m_size;
		// Insert Entity to end of Layer's Array;
		m_entities[m_size] = e;

		//std::cerr << "Entity " << e << " added to layer " << m_id << " with index [" << m_size << "]\n";

		// Update size;
		++m_size;
	}

	void Layer::Remove(Entity e) {
		std::unordered_map<Entity, size_t>::iterator it = m_entityToIndex.find(e);
		if (it == m_entityToIndex.end()) {
			std::cout << "Error: Tried to remove non-existent entity from Layer: " << m_id << '\n';
			return;
		}

		for (size_t i{ it->second }; i + 1 < m_size; ++i) {
			// Shift all entities after Removed one space forward;
			m_entities[i] = m_entities[i + 1];

			// Update Mappings;
			m_entityToIndex[m_entities[i]] = i;
		}
		// Invalidate out-of-scope entity;
		m_entities[m_size - 1] = INVALID_ENTITY;
		// Erase mappings for removed entity;
		m_entityToIndex.erase(e);
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
