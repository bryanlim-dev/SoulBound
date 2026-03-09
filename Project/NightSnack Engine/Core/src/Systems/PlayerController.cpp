/******************************************************************************
 * File:        PlayerController.cpp
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary, 90%)
 *              Bryan Lim Jun Jie (Secondary, 10%)
 *              Clement Ang Yang Ying (Secondary, 10%) - Added coyote time, remapped controls and possession behaviour
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

            // only glide if not grounded
            if (!m_flags.isGrounded) 
            {
                if (vel.y <= -50.f) vel.y = -50.f;

                // play glide sound only once when starting to glide
                if (!m_flags.isGliding) 
                {
                    c.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerGlide", 0.05f);
                    m_flags.isGliding = true;
                }
            }
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

            c.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerJump", 0.4f);


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
            tongue.UpdateOwner(c.GetSystem<PlayerControllerSystem>()->GetPlayer());
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

        abilityName = "Bounce";
        //abilityName = "Grapple";
        Gameplay::Ability Bounce(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;
            if (!c.HasComponent<PlayerController>(target)) return;
            c.GetSystem<PlayerControllerSystem>()->GetBounce().UpdateOwner(c.GetSystem<PlayerControllerSystem>()->GetPlayer());

            //c.GetSystem<AudioSystem>()->PlayRandomVariant("frogTongue", 7);
                    //c.GetSystem<AudioSystem>()->PlaySFX("frogAttach");
            if (!m_flags.bouncing)
            {
                if (c.GetSystem<PlayerControllerSystem>()->GetBounce().Start(c))
                {
                    m_flags.bouncing = true;
                    m_flags.disableMovement = true;
                }
            }
        });
        m_abilities.insert({ abilityName, Bounce });

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

        if (m_flags.bouncing)
        {
            m_bounce.End(m_coord);
        }

        m_coord.ModifyComponent<Transform>(m_player).pos = m_coord.GetComponent<Transform>(m_prevCheckpoint).pos;


        ResetFlags();

        //Reset velocity
        if (m_coord.HasComponent<RigidBody>(m_player))
        {
            m_coord.ModifyComponent<RigidBody>(m_player).vel = Vec2();
        }

    }

    void PlayerControllerSystem::HandleJumpAbilityInput(const InputEvent& event)
    {
        //Only abilities when grounded (or if is bounc, bounce cant jump)

        //Ability 1 (Glide)
        //Only in midair
        //Not too close to ground

        //Ability 2 (Grapple)
        //Only in midair

        //Ability 3 (Bounce)
        //Only when groundedd0d 

        if (event.pressed == InputEvent::InputActionState::Released) {
            m_flags.jumpRequested = false;

            m_flags.abilityKeyRelease = true;
            m_flags.useAbility = false;
            return;
        }

        if ((m_flags.isGrounded && curr_ability != "Bounce") || (CloseToGrounded() && curr_ability == "Glide"))
        {
            if (event.pressed == InputEvent::InputActionState::Pressed ||
                event.pressed == InputEvent::InputActionState::Repeat) {
                if (!m_flags.justJumped)
                    m_flags.jumpRequested = true;
            }
        }
        else
        {
            if (event.pressed == InputEvent::InputActionState::Pressed ||
                event.pressed == InputEvent::InputActionState::Repeat) {
                m_flags.useAbility = true;
            }
            else {
                m_flags.useAbility = false;
            }
        }
    }

    void PlayerControllerSystem::HandlePossessUnpossess()
    {
        Renderable& r = m_coord.ModifyComponent<Renderable>(m_player);

        //Lamda function to try to possess
        auto tryPossess = [&]() {
            // Collision detection between both the entities 
            for (Entity e : m_coord.GetLivingEntities()) {
                auto& et = m_coord.GetComponent<Transform>(e);
                if (!et.isActive) { continue; }

                if (et.name == "Frog" && m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(e, m_player)) {
                    r.materialName = "FrogIdle_1_1"; // depends on the texture name
                    curr_ability = "Grapple";
                    //m_flags.tryPossess = false;
                    m_flags.isPossessing = true;
                    m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerPossess", 0.25f);
                    break;

                }
                else if (et.name == "Ghost" && m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(e, m_player)) {
                    r.materialName = "EyeMove_4_1"; // depends on the texture name
                    curr_ability = "DoubleJump";
                    //m_flags.tryPossess = false;
                    m_flags.isPossessing = true;
                    m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerPossess", 0.25f);
                    break;
                }
                else if (et.name == "Bouncer" && m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(e, m_player)) {
                    r.materialName = "FrogIdle_1_1"; // depends on the texture name
                    curr_ability = "Bounce";
                    //m_flags.tryPossess = false;
                    m_flags.isPossessing = true;
                    break;
                }
                else
                {
                    m_flags.isPossessing = false;
                }
            }
            // If no collision with any entities, still reset it to false
            // m_flags.tryPossess = false; //Is now set outside
        }; //End tryPossess lamda function

        auto tryUnpossess = [&]() {
            // Switch back to the Rain Charm (original player) if it is not in the first place
            if (r.materialName != "SpiritMove_4_1") {
                r.materialName = "SpiritMove_4_1";
                curr_ability = "Glide";
                m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerPossess", 0.25f);
            }
            //// Reset it back anyways
            //m_flags.tryUnPossess = false;
            //m_flags.isPossessing = false;

            if (m_flags.grappling)
            {
                m_frogTongue.End(m_coord);
                m_flags.grappling = false;
            }

            if (m_flags.bouncing)
            {
                m_bounce.End(m_coord);
                m_flags.bouncing = false;
            }
        };//End tryUnpossess lamda function


        //Try to possess first, if fails to find a possessable, unpossess instead
        if (m_flags.possessUnposessActivated)
        {
            tryPossess();
            if (!m_flags.isPossessing) //Failed to possess
                tryUnpossess();

            m_flags.possessUnposessActivated = false;
        }

        //Trying to ONLY possess
        // Player tries to possess an enemy entity
        if (m_flags.tryPossess) {
            tryPossess();
            m_flags.tryPossess = false;
        }

        //Trying to ONLY unpossess
        // Player tries to unpossess and go back to its original form
        if (m_flags.tryUnPossess) {
            tryUnpossess();
            m_flags.tryUnPossess = false;
        }
    }

    void PlayerControllerSystem::CreateGroundedHitbox()
    {
        if (m_closeGroundedHitBox != INVALID_ENTITY)
        {
            m_coord.DestroyEntity(m_closeGroundedHitBox);
            m_closeGroundedHitBox = INVALID_ENTITY;
        }

        m_closeGroundedHitBox = m_coord.CreateEntity();

        if (!m_coord.HasComponent<Transform>(m_closeGroundedHitBox))
            m_coord.AddComponent(m_closeGroundedHitBox, Transform());

        Transform& trans = m_coord.ModifyComponent<Transform>(m_closeGroundedHitBox);
        Transform& playerTrans = m_coord.ModifyComponent<Transform>(m_player);
        trans.name = "CloseToGroundedHitbox";
        trans.scale.x = playerTrans.scale.x / 2;
        trans.scale.y = m_closeGroundedHitBoxHeightOffset;
        trans.pos.y -= m_closeGroundedHitBoxHeightOffset/2;

        m_coord.AddComponent(m_closeGroundedHitBox, Collider{});
        m_coord.AddComponent(m_closeGroundedHitBox, RigidBody{});
        //m_coord.AddComponent(m_closeGroundedHitBox, Renderable{}); //For debug
        m_coord.AddComponent(m_closeGroundedHitBox, Hierarchy{});

        m_coord.AddChildToEntity(m_player, m_closeGroundedHitBox);

        Collider& col = m_coord.ModifyComponent<Collider>(m_closeGroundedHitBox);

        Collider& playerCol = m_coord.ModifyComponent<Collider>(m_player);

        col.type = ColliderType::Trigger;
        col.width = playerCol.width / 2;
        col.height = m_closeGroundedHitBoxHeightOffset;
        col.isStatic = false;
        col.shape = ShapeType::AABB;

        RigidBody& bod = m_coord.ModifyComponent<RigidBody>(m_closeGroundedHitBox);
        bod.useGravity = false;
    }

    void PlayerControllerSystem::UpdateCloseGroundedHitBox()
    {
        if (m_closeGroundedHitBox == INVALID_ENTITY ||
            !m_coord.HasComponent<Transform>(m_closeGroundedHitBox))
            return;

        Transform& trans = m_coord.ModifyComponent<Transform>(m_closeGroundedHitBox);
        
        Vec3 pos = m_coord.GetComponent<Transform>(m_player).pos;
        pos.y -= m_coord.GetComponent<Transform>(m_player).scale.y / 2 + (m_closeGroundedHitBoxHeightOffset/2);
        
        trans.pos = pos;

        Collider& col = m_coord.ModifyComponent<Collider>(m_closeGroundedHitBox);
        //std::cout << "HitBox Colliding Count: " << col.collidedEntities.size() << std::endl;
    }   

    bool PlayerControllerSystem::CloseToGrounded()
    {
        if (m_closeGroundedHitBox == INVALID_ENTITY ||
            !m_coord.HasComponent<Collider>(m_closeGroundedHitBox))
            return false;

        Collider& col = m_coord.ModifyComponent<Collider>(m_closeGroundedHitBox);
        if (col.collidedEntities.size() >= 1)
        {
            for (Entity other : col.collidedEntities)
            {
                if (other == m_player || !m_coord.HasComponent<Collider>(other) || m_coord.GetComponent<Collider>(other).type == ColliderType::Trigger)
                    continue;

                return true;
            }
        }

        return false;
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

        if (m_timeSinceGrounded < m_groundedThreshhold)
            m_flags.justJumped = false;

        if (isGrounded)
        {
            m_timeSinceGrounded = 0;
        }

        //std::cout << "Player Just Jumped : " << m_flags.justJumped << std::endl;
        //std::cout << "Player Time since Grounded : " << m_timeSinceGrounded << "/" << m_groundedThreshhold << std::endl;

        if (m_flags.jumpRequested) {
            // allow jump if pressed right after being off ground
            if (isGrounded || m_timeSinceGrounded < m_groundedThreshhold)
            {
                jump = true;
                m_flags.jumpRequested = false;
            }
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
            m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerJump", 0.4f);

            //m_storedJump = false;
            m_flags.justJumped = true;
            //m_groundedTime = 0.f;

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