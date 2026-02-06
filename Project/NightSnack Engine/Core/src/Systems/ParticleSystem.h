/******************************************************************************
 * File:        ParticleSystem.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Particle System
 *
 * Description:
 * This file contains the declarations of the Particle System.
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
						
						// Examples of starting a Particle Emitter 
						//StartContinuousEmitter(
						//	"Particle-Soil",			// Prefab name
						//	{ 0.f, 0.f, 0.f },			// Spawn position (start position)
						//	{ 100.f, 0.f, 0.f },		// End position
						//	0.f,                        // Spawn radius (unused)
						//	17,                         // Max particles per cycle
						//	35,                         // Spawn delay (steps between particles)
						//	true                        // Should loop
						//);

						//StartContinuousEmitter(
						//	"Particle-OrbitGlow",		// Prefab name
						//	{ 0.f, 0.f, 0.f },			// Spawn position (center)
						//	{},							// End position (unused)
						//	50.f,                       // Spawn radius
						//	30,                         // Max particles per cycle
						//	20,                         // Spawn delay (steps between particles)
						//	true                        // Should loop
						//);

						//StartContinuousEmitter(
						//	"Particle-RisingGlow",      // Prefab name
						//	{ 9.25f, -300.f, 0.f },     // Spawn position (center)
						//	{},							// End position (unused), 
						//	10.f,                       // Spawn radius
						//	20,                         // Max particles per cycle
						//	20,                         // Spawn delay (steps between particles)
						//	true                        // Should loop
						//);

						break;
					}
				}
			});
		}

		/************************************************************************/
		/*!
		\brief
		Initialize function. 
		Register in the map, which particle prefab name is tied to which PARTICLE_TYPE enum type 
		*/
		/************************************************************************/
		void Init();

		/************************************************************************/
		/*!
		\brief
		Main update called by the core.
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Called in the update loop.
		Updates all the active emitters, and decides whether to spawn another single particle or not
		*/
		/************************************************************************/
		void UpdateEmitters();

		/************************************************************************/
		/*!
		\brief
		Called in the update loop.
		Updates all the active particles, their respective Transform components based on their own unique particle patterns

		\param[in] t
		Transform component of the particle entity

		\param[in] p 
		Particle component of the particle entity

		\param[in] e
		The particle entity
		*/
		/************************************************************************/
		void UpdateParticles(Transform& t, Particle& p, Entity e);

		/************************************************************************/
		/*!
		\brief
		Creates a new particle emitter that will spawn particles of its own type in the future

		\param[in] prefabName
		Prefab name of the particle this emitter will spawn

		\param[in] spawnPos
		Spawn position

		\param[in] endPos
		End spawn position. 
		Typically used for falling particles in a horizontal straight line

		\param[in] spawnRadius
		Spawn radius

		\param[in] maxParticles
		Number of particles to spawn in total in 1 loop

		\param[in] spawnDelay
		Number of steps delay, between spawning each particle

		\param[in] shouldLoop
		Loop after spawning 1 cycle of particles or not

		\param[in] followTarget
		The entity that this particle emitter should follow. 
		E.g If this emitter should spawn particles everywhere the player goes, 
		pass this argument as the player entity.

		\param[in] followOffset
		The position offset from the followTarget entity

		\return
		EmitterID of this particle entity, for usage tracking if needed
		*/
		/************************************************************************/
		EmitterID StartContinuousEmitter(const std::string& prefabName, Vec3 spawnPos, Vec3 endPos, float spawnRadius, int maxParticles, int spawnDelay, bool shouldLoop = true, Entity followTarget = INVALID_ENTITY, Vec3 followOffset = {});

		/************************************************************************/
		/*!
		\brief
		Stop a particle emitter

		\param[in] id
		The id of the particle emitter that will be stopped
		*/
		/************************************************************************/
		void StopEmitter(EmitterID id);

		/************************************************************************/
		/*!
		\brief
		Clear all particle emitters in the vector
		*/
		/************************************************************************/
		void StopAllEmitters();

		/************************************************************************/
		/*!
		\brief
		Main update called by the core.
		*/
		/************************************************************************/
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

		/************************************************************************/
		/*!
		\brief
		Spawn a single soil particle with its unique properties from its json file

		\param[in] prefabName
		Prefab name of the particle prefab

		\param[in] leftEndPos
		Left end position of the spawning range

		\param[in] rightEndPos
		Right end position of the spawning range
		*/
		/************************************************************************/
		void SpawnSingleSoilParticle(const std::string& prefabName, Vec3 leftEndPos, Vec3 rightEndPos);

		/************************************************************************/
		/*!
		\brief
		Spawn a single orbiting particle with its unique properties from its json file

		\param[in] prefabName
		Prefab name of the particle prefab

		\param[in] centerPos
		Spawning position
		*/
		/************************************************************************/
		void SpawnSingleOrbitingParticle(const std::string& prefabName, Vec3 centerPos);

		/************************************************************************/
		/*!
		\brief
		Spawn a single rising glow particle with its unique properties from its json file

		\param[in] prefabName
		Prefab name of the particle prefab

		\param[in] centerPos
		Spawning position

		\param[in] spawnRadius
		Spawning radius
		*/
		/************************************************************************/
		void SpawnSingleRisingGlowParticle(const std::string& prefabName, Vec3 centerPos, float spawnRadius);

		/************************************************************************/
		/*!
		\brief
		Register in the particle map, which particle prefab name is tied to which PARTICLE_TYPE enum type 

		\param[in] prefabName
		Prefab name of the particle prefab

		\param[in] type
		PARTICLE_TYPE enum type
		*/
		/************************************************************************/
		void RegisterParticlePrefab(const std::string& prefabName, PARTICLE_TYPE type);

		/************************************************************************/
		/*!
		\brief
		Helper function to get a particle type from the particle's prefab name

		\param[in] prefabName
		Prefab name of the particle prefab

		\return
		PARTICLE_TYPE enum type
		*/
		/************************************************************************/
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

	// Helper functions for randomization for float
	inline float RandomRange(float min, float max) {
		static std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(rng);
	}

	// Helper functions for randomization for int
	inline int RandomRange(int min, int max) {
		static std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(rng);
	}

} // NightSnack