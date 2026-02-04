/******************************************************************************
 * File:        CollectibleSystem.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Collectible System
 *
 * Description:
 * This file contains the definitions of the Collectible System.
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "CollectibleSystem.h"

namespace NightSnack {

	void CollectibleSystem::Init() {

	}

	void CollectibleSystem::Update() {
		if (m_coord.IsPaused()) return;

        m_player = m_coord.GetSystem<PlayerControllerSystem> ()->GetPlayer();


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
                //std::cout << "dj collect not active!!\n";
                continue;
            }

            // Currently active collectibles
            if (m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(m_player, e)) {

                m_coord.GetSystem<AudioSystem>()->PlaySFX("playerCollect");
                // Let the player be able to jump one more time
                if (m_coord.GetComponent<Collectible>(e).type == COLLECTIBLE_TYPE::DOUBLE_JUMP) {
                    m_coord.GetSystem<PlayerControllerSystem>()->EnableDoubleJumpFlag();
                }
                //std::cout << "double jump +1\n";
                // Make the collectible inactive
                t.isActive = false;
            }
        }
	}


} // NightSnack