/******************************************************************************
 * File:        Layer.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * This file defines the Layer class.
 * - Each Layer stores its own set of entities
 * - They can be toggle active and/or visible
 * - Operator < is defined for sorting Layers based on LayerType (found in component.h)
 * 
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

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
		/************************************************************************/
		/*!
		\brief
		Constructor for Layer class that takes the LayerType as a param.
		Fills m_entities with INVALID_ENTITY as all layers start as empty.
		*/
		/************************************************************************/
		Layer(LayerType t)
			: m_type{ t }, m_id{ ++layerCount }, m_isVisible { true }, m_isActive{ true }
		{
			std::fill(m_entities.begin(), m_entities.end(), INVALID_ENTITY);
			m_entityToIndex.reserve(MAX_ENTITIES);
		}

		/************************************************************************/
		/*!
		\brief
		Insert an entity into the layer.
		*/
		/************************************************************************/
		void Insert(Entity e);

		/************************************************************************/
		/*!
		\brief
		Remove an entity from the layer.
		*/
		/************************************************************************/
		void Remove(Entity e);

		/************************************************************************/
		/*!
		\brief
		Checks if an entity is in the layer.
		*/
		/************************************************************************/
		bool Has(Entity e) const noexcept;

		/************************************************************************/
		/*!
		\brief
		Checks if the layer is empty.
		*/
		/************************************************************************/
		bool Empty() const noexcept { return m_size == 0; }

		/************************************************************************/
		/*!
		\brief
		Checks if the layer is active.
		*/
		/************************************************************************/
		bool IsActive() const noexcept { return m_isActive; }

		/************************************************************************/
		/*!
		\brief
		Checks if the layer is visible.
		*/
		/************************************************************************/
		bool IsVisible() const noexcept { return m_isVisible; }

		/************************************************************************/
		/*!
		\brief
		Toggle the layer's visibility status.
		*/
		/************************************************************************/
		void ToggleVisibility() { m_isVisible = !m_isVisible; }

		/************************************************************************/
		/*!
		\brief
		Toggle the layer's active status.
		*/
		/************************************************************************/
		void ToggleActive() { m_isActive = !m_isActive; }

		/************************************************************************/
		/*!
		\brief
		Obtain the Layer's LayerType.
		*/
		/************************************************************************/
		LayerType Type() const { return m_type; }

		/************************************************************************/
		/*!
		\brief
		Obtain the Layer's ID
		*/
		/************************************************************************/
		LayerID ID() const { return m_id; }

		/************************************************************************/
		/*!
		\brief
		Comparator overload for sorting Layers within systems.
		*/
		/************************************************************************/
		bool operator<(Layer const& rhs);

		/************************************************************************/
		/*!
		\brief
		Obtain a reference to the layer's container of entities.
		*/
		/************************************************************************/
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
