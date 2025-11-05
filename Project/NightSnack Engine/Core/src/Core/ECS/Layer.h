#pragma once

#include <unordered_map>
#include <queue>
#include <memory>
#include <set>
#include <optional>
#include <list>
#include <array>
#include <algorithm>

#include "Component.h"
#include "Entity.h"

namespace NightSnack {

	using LayerID = std::uint32_t;
	
	class Layer {

	public:
		Layer(LayerType t)
			: m_type{ t }, m_id{ ++layerCount }, m_isVisible { true }, m_isActive{ true }
		{
			std::fill(m_entities.begin(), m_entities.end(), INVALID_ENTITY);
			m_entityToIndex.reserve(MAX_ENTITIES);
		}

		void Insert(Entity e);
		void Remove(Entity e);
		bool Has(Entity e) const noexcept;
		bool Empty() const noexcept { return m_size == 0; }

		bool IsActive() const noexcept { return m_isActive; }
		bool IsVisible() const noexcept { return m_isVisible; }

		void ToggleVisibility() { m_isVisible = !m_isVisible; }
		void ToggleActive() { m_isActive = !m_isActive; }
		LayerType Type() const { return m_type; }
		LayerID ID() const { return m_id; }

		bool operator<(Layer const& rhs);
		std::array<Entity, MAX_ENTITIES>& Entities() { return m_entities; }

		inline static std::uint32_t layerCount = 0;

	private:
		// Actual storage of Entities.
		std::array<Entity, MAX_ENTITIES> m_entities;
		// Mapping to order entities within Layer;
		std::unordered_map<Entity, size_t> m_entityToIndex;

		size_t m_size{};

		LayerType m_type;
		LayerID m_id;

		bool m_isVisible;
		bool m_isActive;

	};


}	// NightSnack
