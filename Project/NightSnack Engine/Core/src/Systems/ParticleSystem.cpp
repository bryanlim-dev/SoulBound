/******************************************************************************
 * File:        ParticleSystem.cpp
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Particle System
 *
 * Description:
 * This file contains the definitions of the Particle System.
 * Particle System functionalities include:
 * - Updating of all active particle emitters to determine when and where to spawn more particles
 * - Updating of all active particles transform properties
 * - Particle emitters have the option to follow an Entity, to have updated spawn positions instead of just a fixed position to spawn
 * - Each particle prefab entity has its own Particle component, whose values are read from their own JSON file
 * - Upon spawning a particle, most of its properties such as size, velocity and lifetime etc are randomized within a range
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "ParticleSystem.h"

namespace NightSnack {

	void ParticleSystem::Init() {
		// Register the particle prefabs first
		RegisterParticlePrefab("Particle-Soil", PARTICLE_TYPE::SOIL);
		RegisterParticlePrefab("Particle-OrbitGlow", PARTICLE_TYPE::GLOW_ORBIT);
		RegisterParticlePrefab("Particle-RisingGlow", PARTICLE_TYPE::GLOW_RISING);
		RegisterParticlePrefab("Particle-KeyGlow", PARTICLE_TYPE::GLOW_KEY);
	}

	void ParticleSystem::Update() {
		if (m_coord.IsPaused()) return;

		// Update all active emitters
		UpdateEmitters();

		// The particle system should only update the lifetime and transform of entities
		// that have the particle component 
		for (Entity e : m_entities) {
			auto& t = m_coord.ModifyComponent<Transform>(e);
			auto& p = m_coord.ModifyComponent<Particle>(e);

			p.age += m_coord.steps;

			// Particle has reached its lifetime, destroy it
			if (p.age >= p.lifeTime) {
				m_coord.GetSystem<EntityFactory>()->RemoveEntity(e);
				continue;
			}

			// Update the particles' transformations based on their own unique patterns
			UpdateParticles(t, p, e);
		}
	}

	void ParticleSystem::UpdateEmitters() {
		for (auto& emitter : m_emitters) {
			if (!emitter.isActive) continue;

			emitter.stepsSinceLastSpawn += m_coord.steps;

			// Check if it's time to spawn a new particle
			if (emitter.stepsSinceLastSpawn >= emitter.spawnDelay) {
				emitter.stepsSinceLastSpawn = 0;

				// Call each emitter's own spawn function
				emitter.spawnFunction();

				emitter.particlesSpawned++;

				// Check if cycle is complete
				if (emitter.particlesSpawned >= emitter.maxParticles) {
					if (emitter.shouldLoop) {
						// Reset for next cycle
						emitter.particlesSpawned = 0;
					} else {
						// Stop emitter if not looping
						emitter.isActive = false;
					}
				}
			}
		}

		// Remove inactive and finished emitters (typically for burst particles only)
		m_emitters.erase(
			std::remove_if(
				m_emitters.begin(),
				m_emitters.end(),
				[](const ParticleEmitter& e) {
					return !e.isActive && !e.shouldLoop;
				}
			),
			m_emitters.end()
		);
	}

	void ParticleSystem::UpdateParticles(Transform& t, Particle& p, Entity e) {
		switch (p.type) {
		case PARTICLE_TYPE::SOIL:
		{
			// Update gravity acceleration
			if (p.affectedByGravity) {
				p.acceleration.y += -9.8f;
			}

			// Update position
			p.velocity += p.acceleration * FixedDT;
			t.pos += Vec3(p.velocity.x, p.velocity.y, 0.f) * FixedDT;
			t.rot += p.angularVelocity * FixedDT;

			// Size interpolation overtime according to its lifeTime ratio
			//float timeRatio = static_cast<float>(p.age) / p.lifeTime;
			//t.scale = { std::lerp(p.startSize.x, p.endSize.x, timeRatio), std::lerp(p.startSize.y, p.endSize.y, timeRatio) };
		}
		break;

		case PARTICLE_TYPE::GLOW_ORBIT:
		{
			// Update position
			p.orbitAngle += p.orbitSpeed * FixedDT;
			t.pos.x = p.centerPos.x + std::cos(p.orbitAngle) * p.orbitRadius;
			t.pos.y = p.centerPos.y + std::sin(p.orbitAngle) * p.orbitRadius;
			t.rot += p.angularVelocity * FixedDT;

			// Size changes according to the pulse
			float pulse = std::sin(p.age * 0.05f) * 0.5f + 0.5f;
			t.scale = {
				std::lerp(p.startSize.x, p.endSize.x, pulse),
				std::lerp(p.startSize.y, p.endSize.y, pulse)
			};

			if (m_coord.HasComponent<Color>(e)) {
				auto& c = m_coord.ModifyComponent<Color>(e);
				c.a = 0.5f + pulse * 0.5f;
			}
		}
		break;

		case PARTICLE_TYPE::GLOW_RISING:
		case PARTICLE_TYPE::GLOW_KEY:
		{
			// Update position
			t.pos += Vec3(p.velocity.x, p.velocity.y, 0.f) * FixedDT;
			t.rot += p.angularVelocity * FixedDT;

			// Time-based interpolation
			float timeRatio = static_cast<float>(p.age) / p.lifeTime;

			// Size shrinks over time
			t.scale = {
				std::lerp(p.startSize.x, p.endSize.x, timeRatio),
				std::lerp(p.startSize.y, p.endSize.y, timeRatio)
			};

			if (m_coord.HasComponent<Color>(e)) {
				auto& c = m_coord.ModifyComponent<Color>(e);
				float pulse = std::sin(p.age * 0.1f) * 0.15f + 0.85f;
				float baseAlpha = std::lerp(1.0f, 0.4f, timeRatio);
				c.a = baseAlpha * pulse;
				c.a = std::clamp(c.a, 0.f, 1.f);
			}
		}
		break;
		}
	}

	EmitterID ParticleSystem::StartContinuousEmitter(const std::string& prefabName, Vec3 spawnPos, Vec3 endPos, float spawnRadius, int maxParticles, int spawnDelay, bool shouldLoop, Entity followEntity, Vec3 followOffset) {
		ParticleEmitter emitter;
		emitter.id = m_nextEmitterID++;
		emitter.isActive = true;
		emitter.prefabName = prefabName;
		emitter.spawnPos = spawnPos;
		emitter.endPos = endPos;
		emitter.spawnRadius = spawnRadius;
		emitter.maxParticles = maxParticles;
		emitter.spawnDelay = spawnDelay;
		emitter.shouldLoop = shouldLoop;
		emitter.particlesSpawned = 0;
		emitter.stepsSinceLastSpawn = 0;
		emitter.particleType = GetParticleTypeFromPrefabName(prefabName);
		emitter.followEntity = followEntity;
		emitter.followOffset = followOffset;

		// Store the spawn function as a lambda that captures the parameters
		switch (emitter.particleType) {
		case PARTICLE_TYPE::SOIL:
			emitter.spawnFunction = [this, prefabName, spawnPos, endPos]() {
				SpawnSingleSoilParticle(prefabName, spawnPos, endPos);
			};
			break;

		case PARTICLE_TYPE::GLOW_ORBIT:
		{
			EmitterID currentID = emitter.id; // Capture the ID
			emitter.spawnFunction = [this, prefabName, currentID]() {
				// Find the actual emitter in the vector by the ID
				for (auto& em : m_emitters) {
					if (em.id == currentID) {

						// If the followEntity is inactive (Double Jump collectible is collected), 
						// stop this particular emitter
						if (!m_coord.GetComponent<Transform>(em.followEntity).isActive) {
							em.isActive = false;
							break;
						}

						Vec3 finalSpawnPos = em.spawnPos;
						// Final spawn position follows the entity that the emitter is following
						if (em.followEntity != INVALID_ENTITY && m_coord.EntityIsAlive(em.followEntity)) {
							finalSpawnPos = m_coord.GetComponent<Transform>(em.followEntity).pos + em.followOffset;
						} 
						SpawnSingleOrbitingParticle(prefabName, finalSpawnPos);
						break;
					}
				}
				};
		}
			break;

		case PARTICLE_TYPE::GLOW_RISING:
		case PARTICLE_TYPE::GLOW_KEY:
		default:
		{
			EmitterID currentID = emitter.id; // Capture the ID
			emitter.spawnFunction = [this, prefabName, currentID, spawnRadius]() {
				// Find the actual emitter in the vector by the ID
				for (auto& em : m_emitters) {
					if (em.id == currentID) {

						// If the followEntity is inactive (Key collectible is collected), 
						// stop this particular emitter
						if (!m_coord.GetComponent<Transform>(em.followEntity).isActive) {
							em.isActive = false;
							break;
						}

						Vec3 finalSpawnPos = em.spawnPos;
						// Final spawn position follows the entity that the emitter is following
						if (em.followEntity != INVALID_ENTITY && m_coord.EntityIsAlive(em.followEntity)) {
							finalSpawnPos = m_coord.GetComponent<Transform>(em.followEntity).pos + em.followOffset;
						}
						SpawnSingleRisingGlowParticle(prefabName, finalSpawnPos, spawnRadius);
						break;
					}
				}
				};
		}
			break;
		}

		m_emitters.push_back(emitter);
		return emitter.id;
	}

	void ParticleSystem::StopEmitter(EmitterID id) {
		for (auto& emitter : m_emitters) {
			if (emitter.id == id) {
				emitter.isActive = false;
				return;
			}
		}
	}

	void ParticleSystem::StopAllEmitters() {
		for (auto& emitter : m_emitters) {
			emitter.isActive = false;
		}
	}

	void ParticleSystem::ClearAllEmitters() {
		// This function should be called when loading a new level
		StopAllEmitters();
		m_emitters.clear();
	}

	void ParticleSystem::SpawnSingleSoilParticle(const std::string& prefabName, Vec3 leftEndPos, Vec3 rightEndPos) {
		// Calculate spawn position along the line
		Vec3 spawnPos = {
			RandomRange(leftEndPos.x, rightEndPos.x),
			leftEndPos.y,
			0.f
		};

		// Create entity from prefab
		Entity e = m_coord.GetSystem<EntityFactory>()->Create(prefabName);

		// Modify particle properties with randomized values from a range
		auto& p = m_coord.ModifyComponent<Particle>(e);

		Vec2 startSize = p.startSize;
		p.startSize = {
			RandomRange(startSize.x - 10.f, startSize.x + 10.f),
			RandomRange(startSize.y - 10.f, startSize.y + 10.f)
		};
		p.startSize.x = std::max(0.f, p.startSize.x);
		p.startSize.y = std::max(0.f, p.startSize.y);

		Vec2 velocity = p.velocity;
		p.velocity = {
			RandomRange(velocity.x - 10.f, velocity.x + 10.f),
			RandomRange(velocity.y - 10.f, velocity.y + 10.f)
		};

		float rotationAngle = p.angularVelocity;
		p.angularVelocity = RandomRange(-rotationAngle, rotationAngle);

		int lifeTime = p.lifeTime;
		p.lifeTime = RandomRange(lifeTime - 10, lifeTime + 10);

		// Transform component properties
		auto& t = m_coord.ModifyComponent<Transform>(e);
		t.layer = LayerType::FOREGROUND;
		t.pos = spawnPos;
		t.scale = p.startSize;
		t.rot = RandomRange(-rotationAngle, rotationAngle);
		t.isActive = true;
	}

	void ParticleSystem::SpawnSingleOrbitingParticle(const std::string& prefabName, Vec3 centerPos) {
		// Create entity from prefab
		Entity e = m_coord.GetSystem<EntityFactory>()->Create(prefabName);

		// Modify particle properties with randomized values from a range
		auto& p = m_coord.ModifyComponent<Particle>(e);

		Vec2 startSize = p.startSize;
		p.startSize = {
			RandomRange(startSize.x - 1.5f, startSize.x + 1.5f),
			RandomRange(startSize.y - 1.5f, startSize.y + 1.5f)
		};
		p.startSize.x = std::max(0.f, p.startSize.x);
		p.startSize.y = std::max(0.f, p.startSize.y);

		float rotationAngle = p.angularVelocity;
		p.angularVelocity = RandomRange(-rotationAngle, rotationAngle);

		int lifeTime = p.lifeTime;
		p.lifeTime = RandomRange(lifeTime - 10, lifeTime + 10);

		// Spawning position from Emitter side override centerPos in Particle component JSON
		p.centerPos.x = centerPos.x;
		p.centerPos.y = centerPos.y;

		// Set orbit properties
		float orbitRadius = p.orbitRadius;
		p.orbitRadius = RandomRange(orbitRadius * 0.5f, orbitRadius);
		float orbitSpeed = p.orbitSpeed;
		p.orbitSpeed = RandomRange(orbitSpeed - 1.5f, orbitSpeed + 1.5f);
		p.orbitAngle = RandomRange(0.f, 6.28318f);

		// Transform component properties
		// Set transform (initial position on orbit)
		auto& t = m_coord.ModifyComponent<Transform>(e);
		t.layer = LayerType::GAMELAYER;
		t.pos = {
			centerPos.x + std::cos(p.orbitAngle) * p.orbitRadius,
			centerPos.y + std::sin(p.orbitAngle) * p.orbitRadius,
			0.f
		};
		t.scale = p.startSize;
		t.rot = RandomRange(0.f, 360.f);
		t.isActive = true;
	}

	void ParticleSystem::SpawnSingleRisingGlowParticle(const std::string& prefabName, Vec3 centerPos, float spawnRadius) {
		// Calculate spawn position within circular area
		float angle = RandomRange(0.f, 6.28318f);
		float radius = RandomRange(0.f, spawnRadius);
		Vec3 spawnPos = {
			centerPos.x + std::cos(angle) * radius,
			centerPos.y + std::sin(angle) * radius,
			centerPos.z
		};

		Entity e = m_coord.GetSystem<EntityFactory>()->Create(prefabName);

		// Modify particle properties with randomized values from a range
		auto& p = m_coord.ModifyComponent<Particle>(e);

		Vec2 startSize = p.startSize;
		p.startSize = {
			RandomRange(startSize.x, startSize.x + 5.f),
			RandomRange(startSize.y, startSize.y + 5.f)
		};
		p.startSize.x = std::max(0.f, p.startSize.x);
		p.startSize.y = std::max(0.f, p.startSize.y);

		Vec2 endSize = p.endSize;
		p.endSize = {
			RandomRange(endSize.x, endSize.x + 1.5f),
			RandomRange(endSize.y, endSize.y + 1.5f)
		};
		p.endSize.x = std::max(0.f, p.endSize.x);
		p.endSize.y = std::max(0.f, p.endSize.y);

		Vec2 velocity = p.velocity;
		p.velocity = {
			RandomRange(velocity.x - 15.f, velocity.x + 15.f),
			RandomRange(velocity.y - 15.f, velocity.y + 15.f)
		};

		float rotationAngle = p.angularVelocity;
		p.angularVelocity = RandomRange(-rotationAngle, rotationAngle);

		int lifeTime = p.lifeTime;
		p.lifeTime = RandomRange(lifeTime - 20, lifeTime + 20);

		// Transform component properties
		auto& t = m_coord.ModifyComponent<Transform>(e);
		t.layer = LayerType::GAMELAYER;
		t.pos = spawnPos;
		t.scale = p.startSize;
		t.rot = 0.f; //RandomRange(0.f, 360.f);
		t.isActive = true;
	}

	void ParticleSystem::RegisterParticlePrefab(const std::string& prefabName, PARTICLE_TYPE type) {
		m_particlePrefabMap[prefabName] = type;
	}

	PARTICLE_TYPE ParticleSystem::GetParticleTypeFromPrefabName(const std::string& prefabName) {
		auto it = m_particlePrefabMap.find(prefabName);
		if (it != m_particlePrefabMap.end()) {
			return it->second;
		}

		return PARTICLE_TYPE::SMOKE; // default
	}

} // NightSnack