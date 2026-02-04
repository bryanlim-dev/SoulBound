/******************************************************************************
 * File:        CollectibleSystem.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Collectible System
 *
 * Description:
 * This file contains the definitions of the Collectible System.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "CollectibleSystem.h"

namespace NightSnack {

	void CollectibleSystem::Init() {
		std::cout << "hello\n";

	}

	void CollectibleSystem::Update() {
		if (m_coord.IsPaused()) return;

        if (m_player == INVALID_ENTITY) {
            m_player = m_coord.GetSystem<PlayerControllerSystem>()->FindPlayer(m_coord);
            if (m_player == INVALID_ENTITY) return;
        }

        // Check collision between player and collectible
        for (Entity e : m_entities) {
            auto& t = m_coord.ModifyComponent<Transform>(e);
            auto& c = m_coord.ModifyComponent<Collectible>(e);

            // Collectible is inactive
            if (!t.isActive) {
                if (c.willRespawn) {
                    c.currTime += m_coord.steps;

                    // Respawn the collectible
                    if (c.currTime >= c.respawnTimeSteps) {
                        c.currTime = 0;
                        t.isActive = true;
                    }
                }

                continue;
            }

            // Currently active collectibles
            if (m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(m_player, e)) {
                // Let the player be able to jump one more time
                if (m_coord.GetComponent<Collectible>(e).type == COLLECTIBLE_TYPE::DOUBLE_JUMP) {
                    m_coord.GetSystem<PlayerControllerSystem>()->EnableDoubleJumpFlag();
                }

                // Make the collectible inactive
                t.isActive = false;
            }
        }
	}


} // NightSnack