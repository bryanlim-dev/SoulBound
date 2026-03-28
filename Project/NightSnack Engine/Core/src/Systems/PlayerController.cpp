/******************************************************************************
 * File:        PlayerController.cpp
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary, 90%)
 *              Bryan Lim Jun Jie (Secondary, 10%) -  Added respective audio
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
#include <GameLogic/SceneManager.h>
#include "CollectibleSystem.h"

namespace NightSnack {
    PlayerControllerSystem::PlayerControllerSystem(Coordinator& coord, EventDispatcher& disp) : m_coord{ coord }, m_dispatcher{ &disp } {

        m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event)
            {
                switch (event.inputAction) {
                case InputEvent::InputAction::PlayerMoveLeft:
                    if (event.pressed == InputEvent::InputActionState::Pressed ||
                        event.pressed == InputEvent::InputActionState::Repeat) {
                        m_flags.movingLeft = true;
                    }
                    else if (event.pressed == InputEvent::InputActionState::Released) {
                        m_flags.movingLeft = false;
                    }
                    break;
                case InputEvent::InputAction::PlayerMoveRight:
                    if (event.pressed == InputEvent::InputActionState::Pressed ||
                        event.pressed == InputEvent::InputActionState::Repeat) {
                        m_flags.movingRight = true;
                    }
                    else if (event.pressed == InputEvent::InputActionState::Released) {
                        m_flags.movingRight = false;
                    }
                    break;
                case InputEvent::InputAction::PlayerJumpAbility:
                    HandleJumpAbilityInput(event);
                    break;
                    //case InputEvent::InputAction::PlayerSpecialAbility:
                    //    if (event.pressed == InputEvent::InputActionState::Pressed ||
                    //        event.pressed == InputEvent::InputActionState::Repeat) {
                    //        m_flags.useAbility = true;
                    //    }
                    //    else if (event.pressed == InputEvent::InputActionState::Released) {
                    //        m_flags.abilityKeyRelease = true;
                    //        m_flags.useAbility = false;
                    //    }
                    //    else {
                    //        m_flags.useAbility = false;
                    //    }
                    //    break;
                    //case InputEvent::InputAction::PlayerUnPossess:
                    //    if (event.pressed == InputEvent::InputActionState::Pressed) {
                    //        m_flags.tryUnPossess = true;
                    //        curr_ability = "Glide";
                    //    }
                case InputEvent::InputAction::Ability1:
                    if (event.pressed == InputEvent::InputActionState::Pressed && m_coord.GetSystem<NightSnack::Scene::SceneManager>()->GetCheatToggleActive()) {
                        curr_ability = "Grapple";
                        Renderable& r = m_coord.ModifyComponent<Renderable>(m_player);
                        r.materialName = m_abilityDefaultTextures[curr_ability]; // depends on the texture name
                    }
                    break;
                case InputEvent::InputAction::Ability2:
                    if (event.pressed == InputEvent::InputActionState::Pressed && m_coord.GetSystem<NightSnack::Scene::SceneManager>()->GetCheatToggleActive()) {
                        curr_ability = "DoubleJump";
                        Renderable& r = m_coord.ModifyComponent<Renderable>(m_player);
                        r.materialName = m_abilityDefaultTextures[curr_ability]; // depends on the texture name
                    }
                    break;
                case InputEvent::InputAction::Ability3:
                    if (event.pressed == InputEvent::InputActionState::Pressed && m_coord.GetSystem<NightSnack::Scene::SceneManager>()->GetCheatToggleActive()) {
                        curr_ability = "Bounce";
                        Renderable& r = m_coord.ModifyComponent<Renderable>(m_player);
                        r.materialName = m_abilityDefaultTextures[curr_ability]; // depends on the texture name
                        //loadBackMainMenu = true;
                    }
                    break;
                case InputEvent::InputAction::PlayerPossess:
                    if (event.pressed == InputEvent::InputActionState::Pressed) {
                        if (curr_ability == "Bounce" && m_flags.bouncing && m_bounce.GetBounceState() == Bounce::BOUNCE_CHARGING)
                        {
                            m_bounce.End(m_coord);
                            m_flags.bouncing = false;
                            m_flags.disableMovement = false;
                            m_flags.useAbility = false;
                        }
                        else
                        {
                            m_flags.possessUnposessActivated = true;
                        }
                        /*if (!m_flags.isPossessing)
                            m_flags.tryPossess = true;
                        else
                            m_flags.tryUnPossess = true;*/
                    }
                    break;
                }
            });

        //Collision event for checkpoint
        m_dispatcher->Subscribe<CollisionEvent>([this, &coord](const CollisionEvent& event)
            {
                //std::cout << "Checkpoint col check" << std::endl;

                if (!(m_player == (size_t)event.entityA || m_player == (size_t)event.entityB))
                    return;

                Entity checkPoint = INVALID_ENTITY;

                //Check which entity is the checkpoint
                if (coord.HasComponent<Character>(event.entityA) &&
                    std::find(coord.GetComponent<Character>(event.entityA).m_properties.cbegin(), coord.GetComponent<Character>(event.entityA).m_properties.cend(), CHECKPOINT) != coord.GetComponent<Character>(event.entityA).m_properties.cend())
                {
                    checkPoint = event.entityA;
                }
                else if (coord.HasComponent<Character>(event.entityB) &&
                    std::find(coord.GetComponent<Character>(event.entityB).m_properties.cbegin(), coord.GetComponent<Character>(event.entityB).m_properties.cend(), CHECKPOINT) != coord.GetComponent<Character>(event.entityB).m_properties.cend())
                {
                    checkPoint = event.entityB;
                }

                if (checkPoint == INVALID_ENTITY)
                    return;

                //RespawnPlayer();
                m_prevCheckpoint = checkPoint;
                //std::cout << "Checkpoint found Entity: " << checkPoint << std::endl;

            });

        RegisterAbilities();
        AttachAbility();
        prev_ability = curr_ability;

        m_frogTongue.Initalize(m_player, coord, *m_dispatcher);
        m_bounce.Initalize(m_player, coord, *m_dispatcher);
    }

    void PlayerControllerSystem::RegisterAbilities() {

        std::string abilityName = "Glide";
        m_abilityDefaultTextures.insert({ abilityName, "PlayerAbility_4_1" });
        Gameplay::Ability GlideAbility(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            (void)m_flags;
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;


            Vec2& vel = c.ModifyComponent<RigidBody>(target).vel;
            c.ModifyComponent<Renderable>(target).materialName = "PlayerAbility_4_1";

            // only glide if not grounded
            if (!m_flags.isGrounded) 
            {
                if (vel.y <= -50.f) vel.y = -50.f;

                // play glide sound only once when starting to glide
                if (!m_flags.isGliding) 
                {
                    c.GetSystem<AudioSystem>()->PlayRandomVariant("ghostGlide", 3, 0.1f);
                    m_flags.isGliding = true;
                }
            }
        });
        m_abilities.insert({ abilityName, GlideAbility });

        abilityName = "DoubleJump";
        m_abilityDefaultTextures.insert({ abilityName, "EyeMove_4_1" });
        Gameplay::Ability DoubleJumpAbility(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;
            if (!c.HasComponent<PlayerController>(target)) return;
            if (!m_flags.canDoubleJump || m_flags.isGrounded) return;

            Vec2& vel = c.ModifyComponent<RigidBody>(target).vel;
            vel.y = c.GetComponent<PlayerController>(target).jumpSpeed;

            c.GetSystem<AudioSystem>()->PlayRandomVariant("Wings-Reverb", 4, 0.4f);


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
        m_abilityDefaultTextures.insert({ abilityName, "FrogIdle_4_1" });
        Gameplay::Ability GrapplingTongue(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;
            if (!c.HasComponent<PlayerController>(target)) return;

            FrogTongue& tongue = c.GetSystem<PlayerControllerSystem>()->GetFrogTongue();
            Entity player = c.GetSystem<PlayerControllerSystem>()->GetPlayer();
            tongue.UpdateOwner(player);
            c.ModifyComponent<Renderable>(player).materialName = "FrogAbility_4_1";

            if (m_flags.grappling == false)
            {
                //std::cout << "[Frog Grapple] Grappble Looking!" << std::endl;
                //Do distance check to nearest grabbling spot/enemy
                if (tongue.Start(c))
                {
                    c.GetSystem<AudioSystem>()->PlayRandomVariant("frogTongue", 7, 0.75f);
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
        m_abilityDefaultTextures.insert({ abilityName, "SnekIdle_4_1" });
        //abilityName = "Grapple";
        Gameplay::Ability Bounce(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;
            if (!c.HasComponent<PlayerController>(target)) return;
            c.GetSystem<PlayerControllerSystem>()->GetBounce().UpdateOwner(c.GetSystem<PlayerControllerSystem>()->GetPlayer());

            if (!m_flags.bouncing) //Have not started
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

    int PlayerControllerSystem::GetAbilityNumber() const
    {
        if (curr_ability == "Glide")
            return 0;
        else if (curr_ability == "Grapple")
            return 1;
        else if (curr_ability == "DoubleJump")
            return 2;
        else if (curr_ability == "Bounce")
            return 3;

        return -1;
    }

    void PlayerControllerSystem::RespawnPlayer()
    {

        m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("PlayerHurt", 4, 0.25f);

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

        // Force collectibles to respawn immediately
        m_coord.GetSystem<CollectibleSystem>()->ForceRespawnCollectibles();

        bool isGliding = m_flags.isGliding;
        bool isUsingGlide = m_flags.useAbility && m_flags.isGliding;
        bool movingLeft = m_flags.movingLeft;
        bool movingRight = m_flags.movingRight;

        ResetFlags();

        //Preservse these flags
        m_flags.isGliding   = isGliding;
        m_flags.useAbility = isUsingGlide; //Only preserve if gldiing, reset others


        m_flags.movingLeft  = movingLeft;
        m_flags.movingRight = movingRight;


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

            if (curr_ability != "Bounce")
            {
                m_flags.jumpRequested = false;

                m_flags.abilityKeyRelease = true;
                m_flags.useAbility = false;
            }
            else
            {
                //Let bounce handle stuff by itself
            }
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
                (event.pressed == InputEvent::InputActionState::Repeat && curr_ability == "Glide")) {
                m_flags.useAbility = true;
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
                    curr_ability = "Grapple";
                    r.materialName = m_abilityDefaultTextures[curr_ability]; // depends on the texture name
                    //m_flags.tryPossess = false;
                    m_flags.isPossessing = true;
                    m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerPossess", 0.25f);
                    break;

                }
                else if (et.name == "Ghost" && m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(e, m_player)) {
                    curr_ability = "DoubleJump";
                    r.materialName = m_abilityDefaultTextures[curr_ability]; // depends on the texture name
                    //m_flags.tryPossess = false;
                    m_flags.isPossessing = true;
                    m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerPossess", 0.25f);
                    break;
                }
                else if (et.name == "Bouncer" && m_coord.GetSystem<PhysicsSystem>()->EntitiesAreColliding(e, m_player)) {
                    curr_ability = "Bounce";
                    r.materialName = m_abilityDefaultTextures[curr_ability]; // depends on the texture name
                    //m_flags.tryPossess = false;
                    m_flags.isPossessing = true;
                    m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerPossess", 0.25f);
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
                curr_ability = "Glide";
                r.materialName = m_abilityDefaultTextures[curr_ability];
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
                m_flags.disableMovement = false;
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

        //Collider& col = m_coord.ModifyComponent<Collider>(m_closeGroundedHitBox);
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

    void PlayerControllerSystem::DeactivateAbilityBox() {
        if (m_coord.HasComponent<Renderable>(m_abilityBox)) {
            auto& rb = m_coord.ModifyComponent<Renderable>(m_abilityBox);

            if (curr_ability == "DoubleJump") {
                rb.materialName = "AbilityBox-Double_4_1";
            }
            else if (curr_ability == "Grapple") {
                rb.materialName = "AbilityBox-Grapple_4_1";
            }
            else if (curr_ability == "Bounce") {
                rb.materialName = "AbilityBox-Charge_4_1";
            }
            else {
                rb.materialName = "AbilityBox-Glide_4_1";
            }

        }
        if (m_coord.HasComponent<Renderable>(m_abilityKey)) {
            m_coord.ModifyComponent<Renderable>(m_abilityKey).materialName = "AbilityButton";
        }
    }

    void PlayerControllerSystem::ActivateAbilityBox() {

        if (m_coord.HasComponent<Renderable>(m_abilityBox)) {
            auto& rb = m_coord.ModifyComponent<Renderable>(m_abilityBox);

            if (curr_ability == "DoubleJump") {
                rb.materialName = "AbilityBox-DoubleActivated_4_1";
            }
            else if (curr_ability == "Grapple") {
                rb.materialName = "AbilityBox-GrappleActivated_4_1";
            }
            else if (curr_ability == "Bounce") {
                rb.materialName = "AbilityBox-ChargeActivated_4_1";
            }
            else {
                rb.materialName = "AbilityBox-GlideActivated_4_1";
            }

        }
        if (m_coord.HasComponent<Renderable>(m_abilityKey)) {
            m_coord.ModifyComponent<Renderable>(m_abilityKey).materialName = "AbilityButtonActivated";
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

    void PlayerControllerSystem::FindAbilityBox(Coordinator& coord) {
        for (Entity e : coord.GetLivingEntities()) {
            if (coord.GetComponent<Transform>(e).name == "UI-AbilityBox")
                m_abilityBox = e;
            else if (coord.GetComponent<Transform>(e).name == "UI-AbilityKey")
                m_abilityKey = e;
        }
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

    void PlayerControllerSystem::LeftAndRightMovement(Vec2& vel, PlayerController const& pcont)  {
        m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 1.0f;
        auto& body = m_coord.GetComponent<RigidBody>(m_player);

        bool changeAnim = false;
        bool animIdle = false;

        if (m_flags.movingRight && !m_flags.movingLeft) {

            m_flags.facingRight = true;

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
            changeAnim = true;
        }
        else if (m_flags.movingLeft && !m_flags.movingRight) {

            m_flags.facingRight = false;

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
            changeAnim = true;
        }
        else if (!m_flags.movingLeft && !m_flags.movingRight) {

            if (m_flags.isGrounded)
                vel.x = 0.f; // Stop moving when no keys pressed when grounded
            else if (vel.x >= -pcont.moveSpeed && vel.x <= pcont.moveSpeed) //Midair & within player movment
            {
                vel.x = 0.f; // Stop moving when no keys and within speed
            }
            changeAnim = true;
            animIdle = true;

        }
        else if (m_flags.movingLeft && m_flags.movingRight) {

            if (vel.x >= -pcont.moveSpeed && vel.x <= pcont.moveSpeed)
                vel.x = 0.f; // Stop moving when both keys pressed, when moving normally

            if (curr_ability == "Grapple") {
                m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogIdle_4_1";
                m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 1.0f;
            }
        }

        if (changeAnim)
        {

            if (curr_ability == "Bounce" && !m_flags.bouncing) {
                if (!CloseToGrounded() && body.vel.y < 0.f) {
                    m_coord.ModifyComponent<Renderable>(m_player).materialName = "SnekFall_4_1";
                    m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 2.0f;
                }
                else {
                    if (animIdle)
                        m_coord.ModifyComponent<Renderable>(m_player).materialName = "SnekIdle_4_1";
                    else
                        m_coord.ModifyComponent<Renderable>(m_player).materialName = "SnekMove_4_1";

                    m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 2.0f;
                }
            }

            if (curr_ability == "Grapple") {
                if (!CloseToGrounded() && body.vel.y < 0.f) {
                    m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogFall_4_1";
                    m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 1.0f;
                }
                else {
                    if (animIdle)
                        m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogIdle_4_1";
                    else
                        m_coord.ModifyComponent<Renderable>(m_player).materialName = "FrogMove_4_1";
                    m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 2.0f;
                }
            }

            if (curr_ability == "DoubleJump" && !m_flags.bouncing) {
                if (!CloseToGrounded() && body.vel.y < 0.f) {
                    m_coord.ModifyComponent<Renderable>(m_player).materialName = "EyeFall_4_1";
                    m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 2.0f;
                }
                else {
                    if (animIdle)
                        m_coord.ModifyComponent<Renderable>(m_player).materialName = "EyeMove_4_1";
                    else
                        m_coord.ModifyComponent<Renderable>(m_player).materialName = "EyeMove_4_1";
                    m_coord.ModifyComponent<Animator>(m_player).playbackSpeed = 2.0f;
                }
            }
        }

        //std::cout << "Player Vel: " << vel << std::endl;
    }

    void PlayerControllerSystem::Jump(Transform& t, Vec2& vel, PlayerController pcont, bool grounded) {

        bool jump = false;


        bool isGrounded = m_flags.isGrounded;

        // just landed
        if (isGrounded && m_flags.m_wasInAir)
        {
            if (curr_ability == "Bounce")
                m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("SnekLand", 0.55f);
            else if (curr_ability == "Grapple")
                m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("FrogLand", 3, 0.5f);

            m_flags.m_wasInAir = false;
        }

        // track when player leaves ground
        if (!isGrounded)
            m_flags.m_wasInAir = true;


        //std::cout << "Player Just Jumped : " << m_flags.justJumped << std::endl;
        //std::cout << "Player Time since Grounded : " << m_timeSinceGrounded << "/" << m_groundedThreshhold << std::endl;

        if (m_flags.jumpRequested) {
            // allow jump if pressed right after being off ground
            if (grounded)
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
        //if (m_storedJump && grounded)
        //    jump = true;

        if (jump && !m_flags.justJumped) {
            vel.y = pcont.jumpSpeed;
            t.pos.y += 0.5f;
            m_coord.ModifyComponent<Collider>(m_player).isGrounded = false;

            // Play different jump sound based on current ability/form
            if (curr_ability == "DoubleJump") // EyeMove form
                m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("Wings-Reverb", 4, 0.4f);
            else if (curr_ability == "Grapple") // Frog form
                m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("frogJump", 0.4f);
            else // Default spirit form
                m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("ghostJump", 3, 0.4f);

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