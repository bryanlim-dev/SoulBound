/******************************************************************************
 * File:        PlayerController.h
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary, 90%)
 *              Bryan Lim Jun Jie (Secondary, 10%)
 *
 * Description:
 * Controls the Player Entity and sets their Velocity component when key inputs
 * are detected.
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "PlayerController.h"

namespace NightSnack {

    void PlayerControllerSystem::RegisterAbilities() {

        std::string abilityName = "Glide";
        Gameplay::Ability GlideAbility(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            (void)m_flags;
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

        m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("PlayerHurt", 4);

        if (m_prevCheckpoint == INVALID_ENTITY)
            return;

        if (m_flags.grappling)
        {
            m_frogTongue.End(m_coord);
        }

        m_coord.ModifyComponent<Transform>(m_player).pos = m_coord.GetComponent<Transform>(m_prevCheckpoint).pos;


        ResetFlags();

        //Reset velocity
        if (m_coord.HasComponent<RigidBody>(m_player))
        {
            m_coord.ModifyComponent<RigidBody>(m_player).vel = Vec2();
        }

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
        m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 1.0f;

        if (m_flags.movingRight && !m_flags.movingLeft) {

            if (vel.x >= 0)
            {
                //Moving right but within move speed
                if (vel.x <= pcont.moveSpeed)
                    vel.x = pcont.moveSpeed;
                //Moving Right but beyond move speed
                else
                {
                    //Left blank intentionally
                }
            }
            //Moving left
            else
            {
                //Char is moving within move speed, set to 0
                if (vel.x >= -pcont.moveSpeed)
                    vel.x = pcont.moveSpeed;
                //Char is moving beyond the move speed, slow it down
                else 
                    vel.x += pcont.moveSpeed * m_coord.GetSystem<ApplicationSystem>()->GetDT();
            }

            if (curr_ability == "Grapple") {
                m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogMove_4_1";
                m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 2.0f;
            }
        }
        else if (m_flags.movingLeft && !m_flags.movingRight) {
            if (vel.x <= 0)
            {
                //Moving left but within move speed
                if (vel.x >= -pcont.moveSpeed)
                    vel.x = -pcont.moveSpeed;
                //Moving left but beyond move speed
                else
                {
                    //Left blank intentionally
                }
            }
            //Moving right
            else
            {
                //Char is moving within move speed, set to 0
                if (vel.x <= pcont.moveSpeed)
                    vel.x = -pcont.moveSpeed;
                //Char is moving beyond the move speed, slow it down
                else
                    vel.x -= pcont.moveSpeed * m_coord.GetSystem<ApplicationSystem>()->GetDT();
            }


            if (curr_ability == "Grapple") {
                m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogMove_4_1";
                m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 2.0f;
            }
        }
        else if (!m_flags.movingLeft && !m_flags.movingRight) {

            if (m_flags.isGrounded)
                vel.x = 0.f; // Stop moving when no keys pressed when grounded
            else if (vel.x >= -pcont.moveSpeed && vel.x <= pcont.moveSpeed) //Midair & within player movment
            {
                vel.x = 0.f; // Stop moving when no keys and within speed
            }

            if (curr_ability == "Grapple") {
                m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogIdle_4_1";
                m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 1.0f;
            }
        }
        else if (m_flags.movingLeft && m_flags.movingRight) {

            if (vel.x >= -pcont.moveSpeed && vel.x <= pcont.moveSpeed)
                vel.x = 0.f; // Stop moving when both keys pressed, when moving normally

            if (curr_ability == "Grapple") {
                m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogIdle_4_1";
                m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 1.0f;
            }
        }
        //std::cout << "Player Vel: " << vel << std::endl;
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