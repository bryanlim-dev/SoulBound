#pragma once

#include <unordered_map>
#include <queue>
#include <memory>
#include <set>
#include <optional>
#include <list>
#include <algorithm>

#include "Component.h"
#include "Entity.h"

namespace NightSnack {

	// Should layers contain systems themselves? additionally to entities...

	class BackgroundLayer : public Layer {

	};












	enum class LayerType : std::uint8_t {
		_ENGINE,
		BACKGROUND,
		FOREGROUND,
		GAMELAYER,
		GAMEUI,
		EDITOR,
		_ENGINE_POST,

		NOLAYER
	};
	using LayerID = std::uint16_t;
	
	class Layer {

	public:
		Layer(LayerType t, std::string const& n)
			: m_name{ n }, m_type{ t }, isVisible { true } {}

		void Insert(Entity e) { m_entities.insert(e); m_entityOrder.push_back(e); }
		void Remove(Entity e) { 
			m_entities.erase(e); 
			std::list<Entity>::iterator it = std::find(m_entityOrder.begin(), m_entityOrder.end(), e);
			m_entityOrder.erase(it);
		}

		void ChangeName(std::string const& n) { m_name = n; }
		void ToggleVisibility() { isVisible = isVisible ? false : true; }

	private:
		// 
		std::set<Entity> m_entities;
		// order of entities to perform actions -> should be based on hierarchy panel in UI editor;
		std::list<Entity> m_entityOrder;

		std::string m_name;
		LayerType m_type;
		bool isVisible{};

	};

	class LayerManager {

	public:
		void CreateLayer(LayerType = LayerType::GAMELAYER, std::optional<std::string> const&);
		void RemoveLayer(LayerID);

		void ChangeName(LayerID, std::string);

		void ToggleVisibility(LayerID id) { m_layers[id]->ToggleVisibility(); }

		void AddToLayer(Entity e, LayerID);
		void RemoveFromLayer(Entity e, LayerID);

		std::optional<Layer const&> GetLayer(LayerID id) const;
		std::optional<Layer&> GetLayer(LayerID id);

	private:
		struct LayerData {
			Layer const& layer;
			bool valid{ false };
		};

		std::unordered_map<LayerID, std::unique_ptr<Layer>> m_layers;
		std::unordered_map<Entity, LayerData> m_layerCache;

		std::unordered_map<std::string, LayerID> m_nameToID;
		std::unordered_map<LayerID, std::string> m_IDToName;

		std::queue<LayerID> m_availableLayerIDs;

		size_t m_layerCount{};
	};

	class GameLayer : public Layer {

	};

	class EngineLayer : public Layer {

	};



	inline void LayerManager::CreateLayer(LayerType type, std::optional<std::string> const& name) {

		// Name of layer: either custom "Characters" or default "Layer2" etc...
		LayerID id = m_availableLayerIDs.front();
		m_availableLayerIDs.pop();
		std::string layerName = name.value_or(std::string{ "Layer" } + std::to_string(id));

		m_IDToName[id] = layerName;
		m_nameToID[layerName] = id;

		m_layers[m_layerCount] = std::make_unique<Layer>(type, layerName);
		++m_layerCount;
	}

	inline void LayerManager::RemoveLayer(LayerID id) {

		if (m_layers.find(id) == m_layers.end()) {
			std::cerr << "Error: Layer not found, cannot be removed!\n";
			return;
		}

		// remove layer and reinsert ID into queue of available IDs;
		m_layers.erase(id);
		m_availableLayerIDs.push(id);
		--m_layerCount;

		// remove mappings of name and ID;
		m_nameToID.erase(m_IDToName[id]);
		m_IDToName.erase(id);
	}

	inline void LayerManager::ChangeName(LayerID id, std::string n) {

		if (m_layers.find(id) == m_layers.end()) {
			std::cerr << "Error: Layer not found, name cannot be changed!\n";
			return;
		}
		if (m_nameToID.find(n) != m_nameToID.end()) {
			std::cerr << "Error: Layer name " << n << " is already in use!\n";
			return;
		}

		m_layers[id]->ChangeName(n);
		m_IDToName[id] = n;

		m_nameToID.erase(n);
		m_nameToID[n] = id;
	}

	inline std::optional<Layer const&> LayerManager::GetLayer(LayerID id) const {

		if (m_layers.find(id) == m_layers.end()) {
			std::cerr << "Error: Layer not found!\n";
			return std::nullopt;
		}
		return *(m_layers.at(id));
	}

	inline std::optional<Layer&> LayerManager::GetLayer(LayerID id) {

		if (m_layers.find(id) == m_layers.end()) {
			std::cerr << "Error: Layer not found!\n";
			return std::nullopt;
		}
		return *(m_layers.at(id));
	}

	inline void LayerManager::AddToLayer(Entity e, LayerID id) {

		if (m_layers.find(id) == m_layers.end()) {
			std::cerr << "Error: Layer does not exist, cannot insert Entity " << e << "!\n";
			return;
		}
		m_layers[id]->Insert(e);
	}

	inline void LayerManager::RemoveFromLayer(Entity e, LayerID id) {

		if (m_layers.find(id) == m_layers.end()) {
			std::cerr << "Error: Layer does not exist, cannot remove Entity " << e << "!\n";
			return;
		}
		m_layers[id]->Remove(e);
	}

}	// NightSnack
