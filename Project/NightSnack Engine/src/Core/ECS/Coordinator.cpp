/******************************************************************************
 * File:        Coordinator.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * Defines all member functions (non-templated) of Coordinator ECS interface.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Coordinator.h"
#include "Systems/Graphics.h"

namespace NightSnack {

	void Coordinator::Initialize(/*EventDispatcher disp*/) /*: m_dispatcher{disp}*/ {
		// Create ECS Managers;
		m_entityManager = std::make_unique<EntityManager>();
		m_systemManager = std::make_unique<SystemManager>();
		m_componentManager = std::make_unique<ComponentManager>();

	}

	// ------------------------------------------------------------------------------------------------
	// ECS INTERFACE;
	// ------------------------------------------------------------------------------------------------

	Entity Coordinator::CreateEntity() {
		// Returns the ID of created Entity;
		return m_entityManager->CreateEntity();
	}

	// Destroy an Entity, remove it from all Systems and ComponentArrays;
	void Coordinator::DestroyEntity(Entity e) {
		if (e == INVALID_ENTITY) { return; }

		if (HasComponent<Renderable>(e)) {
			GetSystem<GraphicsSystem>()->RemoveCache(e);
		}

		// Remove from Live Entites;
		m_entityManager->DestroyEntity(e);
		// Remove Entity from Components;
		m_componentManager->EntityDestroyed(e);
		// Remove Entity from all Systems;
		m_systemManager->EntityDestroyed(e);
	}

	int Coordinator::GetWindowHeight() {
		return GetSystem<ApplicationSystem>()->GetHeight();
	}

	int Coordinator::GetWindowWidth() {
		return GetSystem<ApplicationSystem>()->GetWidth();
	}

}	// NightSnack
