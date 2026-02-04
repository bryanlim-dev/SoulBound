#include "PlayerController.h"

namespace NightSnack {

    void PlayerControllerSystem::RegisterAbilities() {

        std::string abilityName = "Glide";
        Gameplay::Ability GlideAbility(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;

            Vec2& vel = c.ModifyComponent<RigidBody>(target).vel;
            if (vel.y <= -50.f) vel.y = -50.f;

            //// Only play if not already playing
            //if (!m_flags.isGlideSoundPlaying) {
            //    c.GetSystem<AudioSystem>()->PlaySFX("playerGlide");
            //    m_flags.isGlideSoundPlaying = true;
            //}

            });
        m_abilities.insert({ abilityName, GlideAbility });

        abilityName = "DoubleJump";
        Gameplay::Ability DoubleJumpAbility(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;
            if (!c.HasComponent<PlayerController>(target)) return;
            if (!m_flags.canDoubleJump || m_flags.isGrounded) return;

            Vec2& vel = c.ModifyComponent<RigidBody>(target).vel;
            vel.y = c.GetComponent<PlayerController>(target).jumpSpeed;

            c.GetSystem<AudioSystem>()->PlaySFX("playerJump");

            // Start the emitter to spawn particles
            // These particles spawn position will be based on the updated player position
            c.GetSystem<ParticleSystem>()->StartContinuousEmitter(
                "Particle-RisingGlow",      // Prefab name
                {},                         // Spawn position (center)
                {},							// End position (unused)
                10.f,                       // Spawn radius
                10,                          // Max particles per cycle
                1,                          // Spawn delay (steps between particles)
                false,                      // Should loop
                m_flags.m_playerID            // Follow entity
            );

            m_flags.timeSinceLastDoubleJump = 0.f;
            m_flags.canDoubleJump = false;
            m_flags.useAbility = false;

        });
        m_abilities.insert({ abilityName, DoubleJumpAbility });

        abilityName = "Grapple";
        Gameplay::Ability GrapplingTongue(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;
            if (!c.HasComponent<PlayerController>(target)) return;

            FrogTongue& tongue = c.GetSystem<PlayerControllerSystem>()->GetFrogTongue();
            if (m_flags.grappling == false)
            {
                //std::cout << "[Frog Grapple] Grappble Looking!" << std::endl;
                //Do distance check to nearest grabbling spot/enemy
                if (tongue.Start(c))
                {
                    c.GetSystem<AudioSystem>()->PlayRandomVariant("frogTongue", 7);
                    //Valid target
                    m_flags.grappling = true;

                    c.GetSystem<AudioSystem>()->PlaySFX("frogAttach");


                }
                //else
                //    std::cout << "[Frog Grapple] Grappble NONE!" << std::endl;

            }

            if (m_flags.grappling)
            {
                if (!tongue.Update(c))
                {
                    m_flags.grappling = false;
                    tongue.End(c);
                }
            }

        });
        m_abilities.insert({ abilityName, GrapplingTongue });

    }

    bool PlayerControllerSystem::AttachAbility() {
        curr_ability = "Glide";
        return true;
    }

    void PlayerControllerSystem::RespawnPlayer()
    {
        if (m_prevCheckpoint == INVALID_ENTITY)
            return;

        m_coord.ModifyComponent<Transform>(m_player).pos = m_coord.GetComponent<Transform>(m_prevCheckpoint).pos;

        ResetFlags();
    }

    bool PlayerControllerSystem::UseSpecialAbility() {

        std::unordered_map<std::string, Gameplay::Ability>::iterator it = m_abilities.find(curr_ability);
        if (it == m_abilities.end()) {
            std::cerr << "[PlayerControl] Tried to use non-registered ability!\n";
            return false;
        }
        it->second(m_coord, m_player, m_flags);
        return true;
    }

    Entity PlayerControllerSystem::FindPlayer(Coordinator& coord) {
        // Camera Signature (only have components Camera and Tranform)
        Signature playerSig;

        playerSig.set(GetComponentTypeID<PlayerController>());
        playerSig.set(GetComponentTypeID<Transform>());
        playerSig.set(GetComponentTypeID<RigidBody>());
        playerSig.set(GetComponentTypeID<Collider>());

        // Iterate through all living Entities in the COORDINATOR instead
        // Since the GraphicsSystem will not have a Camera Entity
        for (Entity e : coord.GetLivingEntities()) {
            if ((coord.GetEntitySignature(e) & playerSig) == playerSig) { // This camera Entity is the main camera
                return e;
            }
        }
        //std::cerr << "Error: No Player Entity found in scene!\n";
        return INVALID_ENTITY;
    }

    void PlayerControllerSystem::LeftAndRightMovement(Vec2& vel, PlayerController const& pcont) const {

        if (m_flags.movingRight && !m_flags.movingLeft) {
            vel.x = pcont.moveSpeed;
        }
        else if (m_flags.movingLeft && !m_flags.movingRight) {
            vel.x = -pcont.moveSpeed;
        }
        else if (!m_flags.movingLeft && !m_flags.movingRight) {
            vel.x = 0.f; // Stop moving when no keys pressed
        }
        else if (m_flags.movingLeft && m_flags.movingRight) {
            vel.x = 0.f; // Stop moving when both keys pressed
        }
    }

    void PlayerControllerSystem::Jump(Transform& t, Vec2& vel, PlayerController pcont, bool isGrounded) {

        bool jump = false;
        m_timeSinceGrounded += FixedDT;
        m_groundedTime += FixedDT;
        if (m_groundedTime > m_repeatJumpThreshhold)
            m_flags.justJumped = false;
        if (isGrounded)
        {
            m_timeSinceGrounded = 0;

        }

        if (m_flags.jumpRequested) {
            // allow jump if isgrounded
            if (isGrounded && m_groundedTime > m_repeatJumpThreshhold) {
                jump = true;
            }
            // allow jump if pressed right after being off ground
            if (!isGrounded && m_timeSinceGrounded < m_groundedThreshhold && !m_flags.justJumped)
                jump = true;
        }

        //// allow jump if pressed right before being grounded
        //if (m_flags.jumpRequested && !jump)
        //    m_storedJump = true;
        //if (m_storedJump) {
        //    m_storedJumpTimer += FixedDT;
        //    if (m_storedJumpTimer > m_storedJumpThreshold) {
        //        m_storedJump = false;
        //        m_storedJumpTimer = 0.f;
        //    }
        //}
        //if (m_storedJump && isGrounded)
        //    jump = true;

        if (jump && !m_flags.justJumped) {
            vel.y = pcont.jumpSpeed;
            t.pos.y += 0.5f;
            m_coord.ModifyComponent<Collider>(m_player).isGrounded = false;
            m_coord.GetSystem<AudioSystem>()->PlaySFX("playerJump");

            //m_storedJump = false;
            m_flags.justJumped = true;
            m_groundedTime = 0.f;

            // Start the emitter to spawn particles
            // These particles spawn position will be based on the updated player position
            m_playerJumpEmitter = m_coord.GetSystem<ParticleSystem>()->StartContinuousEmitter(
                "Particle-RisingGlow",      // Prefab name
                {},                         // Spawn position (center)
                {},							// End position (unused)
                10.f,                       // Spawn radius
                10,                          // Max particles per cycle
                1,                          // Spawn delay (steps between particles)
                false,                      // Should loop
                m_player                    // Follow entity
            );

            m_flags.canDoubleJump = true;
        }
    }
    PlayerFlags& PlayerControllerSystem::GetPlayerFlags()
    {
        return m_flags;
    }
}