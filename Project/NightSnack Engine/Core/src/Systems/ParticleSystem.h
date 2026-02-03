/******************************************************************************
 * File:        ParticleSystem.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Particle System
 *
 * Description:
 * This file contains the declarations of the Particle System.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Factory/EntityFactory.h"
#include <random>

namespace NightSnack {

	using EmitterID = uint32_t;

	class ParticleSystem : public ISystem {

	public:

		

		/************************************************************************/
		/*!
		\brief
		Constructor for the ParticleSystem

		\param[in] coord
		Reference to the ECS Coordinator

		\param[in] dispatcher
		Reference to the event dispatcher used for Input Events
		*/
		/************************************************************************/
		ParticleSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
			m_systemSignature.set(GetComponentTypeID<Renderable>());
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Particle>());

			Init();

			// Dispatcher lambda

			// Input event
			// Key press to change a single entity animation
			m_dispatcher->Subscribe<InputEvent>([this](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::ChangeAnimation:
						
						StartContinuousEmitter(
							"Particle-Soil",			// Prefab name
							{ 0.f, 0.f, 0.f },			// Spawn position (start position)
							{ 100.f, 0.f, 0.f },		// End position
							0.f,                        // Spawn radius (unused)
							17,                         // Max particles per cycle
							35,                         // Spawn delay (steps between particles)
							true                        // Should loop
						);

						StartContinuousEmitter(
							"Particle-OrbitGlow",		// Prefab name
							{ 0.f, 0.f, 0.f },			// Spawn position (center)
							{},							// End position (unused)
							50.f,                       // Spawn radius
							30,                         // Max particles per cycle
							20,                         // Spawn delay (steps between particles)
							true                        // Should loop
						);

						StartContinuousEmitter(
							"Particle-RisingGlow",      // Prefab name
							{ 9.25f, -300.f, 0.f },     // Spawn position (center)
							{},							// End position (unused), 
							10.f,                       // Spawn radius
							20,                         // Max particles per cycle
							20,                         // Spawn delay (steps between particles)
							true                        // Should loop
						);

						break;
					}
				}
			});
		}

		void Init();

		/************************************************************************/
		/*!
		\brief
		Main update called by the core.
		*/
		/************************************************************************/
		void Update() override;

		void UpdateEmitters();

		void UpdateParticles(Transform& t, Particle& p, Entity e);

		// endPos - Used for SOIL (right end position), unused for others
		// spawnRadius - Used for RISING_GLOW (circular area), unused for others
		EmitterID StartContinuousEmitter(const std::string& prefabName, Vec3 spawnPos, Vec3 endPos, float spawnRadius, int maxParticles, int spawnDelay, bool shouldLoop = true, Entity followTarget = INVALID_ENTITY, Vec3 followOffset = {});

		void StopEmitter(EmitterID id);
		void StopAllEmitters();
		void ClearAllEmitters();

		/************************************************************************/
		/*!
		\brief
		Get the system's signature.
		Required by ISystem inheritance

		\return
		Signature (set of required components)
		*/
		/************************************************************************/
		Signature GetSignature() const override { return m_systemSignature; }

		/************************************************************************/
		/*!
		\brief
		Gets the name of this system, for debugging purposes
		Required by ISystem inheritance

		\return
		Name of the system
		*/
		/************************************************************************/
		std::string GetName() const { return "ParticleSystem"; }

	private:

		void SpawnSingleSoilParticle(const std::string& prefabName, Vec3 leftEndPos, Vec3 rightEndPos);
		void SpawnSingleOrbitingParticle(const std::string& prefabName, Vec3 centerPos);
		void SpawnSingleRisingGlowParticle(const std::string& prefabName, Vec3 centerPos, float spawnRadius);

		void RegisterParticlePrefab(const std::string& prefabName, PARTICLE_TYPE type);
		PARTICLE_TYPE GetParticleTypeFromPrefabName(const std::string& prefabName);

		struct ParticleEmitter {
			EmitterID id;
			bool isActive = false;
			int particlesSpawned = 0;
			int maxParticles = 1;
			int spawnDelay = 1;
			int stepsSinceLastSpawn = 0;
			bool shouldLoop = false;

			// Spawn configuration
			std::string prefabName{};  // Which prefab to spawn
			PARTICLE_TYPE particleType = PARTICLE_TYPE::SMOKE; // Type of particle
			Vec3 spawnPos{};           // Center/left position for spawning
			Vec3 endPos{};			 // Right position for spawning (SOIL only for now)
			float spawnRadius = 1.f;       // Radius of circular spawn area

			// Follow target for updated spawning position
			Entity followEntity = INVALID_ENTITY;
			Vec3 followOffset{};

			// Store the spawn function
			std::function<void()> spawnFunction;
		};

		Signature m_systemSignature{};
		// Reference to Coordinator;
		Coordinator& m_coord;
		// System event dispatcher
		EventDispatcher* m_dispatcher = nullptr;

		std::vector<ParticleEmitter> m_emitters;
		std::unordered_map<std::string, PARTICLE_TYPE> m_particlePrefabMap;
		EmitterID m_nextEmitterID = 1;
	};

	inline float RandomRange(float min, float max) {
		static std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(rng);
	}

	inline int RandomRange(int min, int max) {
		static std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(rng);
	}

} // NightSnack