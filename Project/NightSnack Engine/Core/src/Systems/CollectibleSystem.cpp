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

	void CollectibleSystem::Update() {
		if (m_coord.IsPaused()) return;

		m_player = m_coord.GetSystem<PlayerControllerSystem>()->GetPlayer();

		if (!m_createdParticleEmitters) {
			CreateParticleEmitters();
			m_createdParticleEmitters = true;
		}

		if (!m_getFlameCollectibleIcons) {
			m_getFlameCollectibleIcons = GetFlameCollectibleIcons();
		}

		// Check collision between player and collectible
		for (Entity e : m_entities) {
			auto& t = m_coord.ModifyComponent<Transform>(e);
			auto& c = m_coord.ModifyComponent<Collectible>(e);

			if (t.layer == LayerType::GAMEUI) {
				continue;
			}

			// Collectible is inactive
			if (!t.isActive) {
				if (c.willRespawn) {
					c.currTime += m_coord.steps;

					// Respawn the collectible
					if (c.currTime >= c.respawnTimeSteps) {
						c.currTime = 0;
						t.isActive = true;

						// Create the particle emitters again here to spawn 
						if (c.type == COLLECTIBLE_TYPE::DOUBLE_JUMP) {
							// Double jump collectibles will spawn orbiting particles
							m_coord.GetSystem<ParticleSystem>()->StartContinuousEmitter(
								"Particle-OrbitGlow",      // Prefab name
								t.pos,                         // Spawn position (center)
								{},							// End position (unused)
								{},                       // Spawn radius (override by orbit radius in json)
								30,                          // Max particles per cycle
								5,                          // Spawn delay (steps between particles)
								true,                     // Should loop
								e
							);
						}
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
				} else if (m_coord.GetComponent<Collectible>(e).type == COLLECTIBLE_TYPE::KEY) {
					++m_numFlamesCollected;
					LightUpFlameCollectibleIcon();
				}
				//std::cout << "double jump +1\n";
				// Make the collectible inactive
				t.isActive = false;
			}
		}
	}

	void CollectibleSystem::CreateParticleEmitters() {
		for (Entity e : m_entities) {
			auto& t = m_coord.ModifyComponent<Transform>(e);
			auto& c = m_coord.ModifyComponent<Collectible>(e);

			if (!t.isActive) { continue; }

			if (c.type == COLLECTIBLE_TYPE::DOUBLE_JUMP) {
				// Double jump collectibles will spawn orbiting particles
				m_coord.GetSystem<ParticleSystem>()->StartContinuousEmitter(
					"Particle-OrbitGlow",      // Prefab name
					t.pos,                         // Spawn position (center)
					{},							// End position (unused)
					{},                       // Spawn radius (override by orbit radius in json)
					30,                          // Max particles per cycle
					5,                          // Spawn delay (steps between particles)
					true,                     // Should loop
					e
				);
			} else if (c.type == COLLECTIBLE_TYPE::KEY) {
				// Key collectibles will spawn rising glow particles
				m_coord.GetSystem<ParticleSystem>()->StartContinuousEmitter(
					"Particle-KeyGlow",      // Prefab name
					t.pos,                         // Spawn position (center)
					{},							// End position (unused)
					{},                       // Spawn radius (override by orbit radius in json)
					20,                          // Max particles per cycle
					15,                          // Spawn delay (steps between particles)
					true,                     // Should loop
					e
				);
			}
		}
	}

	bool CollectibleSystem::GetFlameCollectibleIcons() {
		m_flameCollectibleIcons.clear();
		
		for (Entity e : m_entities) {
			auto& t = m_coord.GetComponent<Transform>(e);
			if (!t.isActive) continue;

			// Only entities on the GameUI layer and is of type ICON
			if (t.layer == LayerType::GAMEUI && m_coord.GetComponent<Collectible>(e).type == COLLECTIBLE_TYPE::ICON) {
				m_flameCollectibleIcons.push_back(e);
			}
		}

		return !m_flameCollectibleIcons.empty();
	}

	void CollectibleSystem::LightUpFlameCollectibleIcon() {
		if (m_flameCollectibleIcons.empty()) {
			//std::cout << "Flame Collectible Icons vector is empty!!!\n";
			return;
		}
		Entity currFlame = m_flameCollectibleIcons[m_numFlamesCollected - 1];

		// Update the texture of the current flame entity icon
		if (m_coord.HasComponent<Renderable>(currFlame)) {
			auto& r = m_coord.ModifyComponent<Renderable>(currFlame);
			r.materialName = "TitleFireAnim_4_1";

		} else {
			//std::cout << "Current Flame Icon does not have a Renderable component!!!\n";
			return;
		}
	}

	void CollectibleSystem::SpawnCollectibleParticles() {
		// This function is called whenever the level loads
		m_createdParticleEmitters = false;
		m_getFlameCollectibleIcons = false;
		m_numFlamesCollected = 0;
	}

	void CollectibleSystem::ToggleCollectibleIcons(bool enable) {
		for (Entity e : m_flameCollectibleIcons) {
			auto& t = m_coord.ModifyComponent<Transform>(e);
			t.isActive = enable;
		}
	}

} // NightSnack