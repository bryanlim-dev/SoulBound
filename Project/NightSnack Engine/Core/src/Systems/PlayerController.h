/******************************************************************************
 * File:        PlayerController.h
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary, 70%)
 *              Bryan Lim Jun Jie (Secondary, 10%) - Added respective audio  
 *              Goh Bei Lai (Secondary, 10%) - Added player possession mechanic 
 *              Clement Ang Yang Ying (Secondary, 10%) - Added coyote time, remapped controls and possession behaviour
 *
 * Description:
 * Controls the Player Entity and sets their Velocity component when key inputs
 * are detected.
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/Input.h"
#include "Systems/AudioSystem.h"
#include <Factory/EntityFactory.h>
#include <GameLogic/FrogTongue.h>
#include "Systems/ParticleSystem.h"
#include "Systems/Physics/Physics.h"
#include <GameLogic/Bounce.h>
#include "Systems/DialogueSystem.h"

struct PlayerFlags {
    bool movingLeft = false;
    bool movingRight = false;
    bool facingRight = true;

    bool jumpRequested = false;
    bool justJumped{};

    bool disableMovement = false;

    bool playerChange = false;
    bool isGrounded = false;

    bool isPossessing = false;
    bool useAbility = false;

    bool grappling = false;
    bool bouncing = false;

    bool canDoubleJump = true;
    bool abilityKeyRelease = false;
    float timeSinceLastDoubleJump = 0.f;

    NightSnack::Entity m_playerID;

    bool possessUnposessActivated = false; //Player pressed possess/unpossess buttton
    bool tryPossess = false;               //Trying to ONLY possess
    bool tryUnPossess = false;             //Trying to ONLY unpossess 
    // add more flags if needed

    bool isGlideSoundPlaying = false;
    bool isGliding = false;

    bool isWalking = false;
    float walkStepTimer = 0.f;
};

namespace NightSnack::Gameplay {
    class Ability {
    public:
        // Constructors
        Ability(std::string n, void(*func)(Coordinator& c, Entity e, PlayerFlags& m_flags))
            : m_name{ n }, m_callable{ func } {}
        Ability(Ability const& a) {
            m_name = a.m_name;
            m_callable = a.m_callable;
        }
        // Invoke callable to activate ability
        void operator()(Coordinator& c, Entity tgt, PlayerFlags& flags) { m_callable(c, tgt, flags); }
    private:
        std::string m_name;
        void(*m_callable)(Coordinator& c, Entity tgt, PlayerFlags& m_flags);

    };
}

namespace NightSnack {

    class PlayerControllerSystem : public ISystem {

    public:
        /************************************************************************/
        /*!
        \brief
        Constructor for PlayerControllerSystem class taht sets up the system's
        signature and initializes the current player (m_player).

        \param coord
        Reference to the Coordinator.
        \param disp
        Reference to the Event Dispatcher.
        */
        /************************************************************************/
        PlayerControllerSystem(Coordinator& coord, EventDispatcher& disp) : m_coord{ coord }, m_dispatcher { &disp } {

            m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event)
                {
                    switch (event.inputAction) {
                    case InputEvent::InputAction::PlayerMoveLeft:
                        if (event.pressed == InputEvent::InputActionState::Pressed ||
                            event.pressed == InputEvent::InputActionState::Repeat) {
                            m_flags.movingLeft = true;
                            m_flags.facingRight = false;
                        }
                        else if (event.pressed == InputEvent::InputActionState::Released) {
                            m_flags.movingLeft = false;
                        }
                        break;
                    case InputEvent::InputAction::PlayerMoveRight:
                        if (event.pressed == InputEvent::InputActionState::Pressed ||
                            event.pressed == InputEvent::InputActionState::Repeat) {
                            m_flags.movingRight = true;
                            m_flags.facingRight = true;
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
                    /*case InputEvent::InputAction::Ability1:
                        if (event.pressed == InputEvent::InputActionState::Pressed) {
                            curr_ability = "Glide";
                        }
                        break;*/
                    case InputEvent::InputAction::Ability2:
                        if (event.pressed == InputEvent::InputActionState::Pressed) {
                            curr_ability = "Grapple";
                        }
                        break;
                    case InputEvent::InputAction::Ability3:
                        if (event.pressed == InputEvent::InputActionState::Pressed) {
                            curr_ability = "DoubleJump";
                            loadBackMainMenu = true;
                        }
                        break;
                    case InputEvent::InputAction::PlayerPossess:
                        if (event.pressed == InputEvent::InputActionState::Pressed) {
                            m_flags.possessUnposessActivated = true;
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

        /************************************************************************/
        /*!
        \brief
        Updates the current player.
        Sets Velocity component of Player entity when Inputs are triggered.

        \param dt
        delta time.
        \param coord
        Reference to Coordinator.
        */
        /************************************************************************/
        void Update() override {

            static bool emitted = false;
            if (m_coord.IsPaused()) {
                //emitted = false;
                //m_coord.GetSystem<ParticleSystem>()->StopEmitter(m_playerJumpEmitter);
                return;
            }

            m_player = FindPlayer(m_coord);
            m_flags.m_playerID = m_player;
            if (m_player == INVALID_ENTITY) { return; }

            // Skip Player Update if camera is currently in a panning animation
            if (m_coord.GetSystem<CameraSystem>()->GetCameraIsPanning()) {
                return;
            }

            // Skip Player Update if dialogue system is active
            if (m_coord.GetSystem<DialogueSystem>()->IsActive()) {
                // Reset player velocity if it was moving right before a dialogue system started
                m_coord.ModifyComponent<RigidBody>(m_player).vel = {};
                return;
            }

            if (reset) {
                AttachAbility();
                m_flags = PlayerFlags{};

                m_bounce.Reset(m_coord);

                reset = false;
                m_pauseInputs = false;

                CreateGroundedHitbox();
            }

            if (m_pauseInputs) return;

            UpdateCloseGroundedHitBox();

            PlayerController const& pcont = m_coord.GetComponent<PlayerController>(m_player);
            Vec2& vel = m_coord.ModifyComponent<RigidBody>(m_player).vel;
            Transform& t = m_coord.ModifyComponent<Transform>(m_player);
            Collider& c = m_coord.ModifyComponent<Collider>(m_player);
            Renderable& r = m_coord.ModifyComponent<Renderable>(m_player);

            // Basic movement -----------------------------
            if (!m_flags.disableMovement)
            {
                LeftAndRightMovement(vel, pcont);
                Jump(t, vel, pcont, c.isGrounded);
            }


            if (m_flags.bouncing == true)
            {
                if (!m_bounce.IsActive() || !m_bounce.Update(m_coord))
                {
                    m_flags.disableMovement = false;
                    m_flags.bouncing = false;

                    m_bounce.End(m_coord);
                }
            }

            //
            static float time = 0.f;
            if (vel.y == 0.f) {
                time += FixedDT;
            }
            else {
                time = 0.f;
            }
            if (time >= 0.3f) {
                c.isGrounded = true;
                time = 0.f;
            }
            bool isGrounded = m_flags.isGrounded = c.isGrounded || m_timeSinceGrounded < m_groundedThreshhold;

			// reset glide when grounded
            if (isGrounded && m_flags.isGliding) 
            {
                m_flags.isGliding = false;
            }

            bool movingHorizontally = (m_flags.movingLeft ^ m_flags.movingRight);
            bool isFrog = (r.materialName == "FrogMove_4_1");

            bool shouldWalk =
                isFrog &&
                isGrounded &&
                movingHorizontally &&
                !m_flags.disableMovement;

            if (shouldWalk)
            {
                // if just started walking, play immediately
                if (!m_flags.isWalking)
                {
                    m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("frogWalk", 3, 0.15f);
                    m_flags.walkStepTimer = 0.0f;
                    m_flags.isWalking = true;
                }

                // accumulate time
                m_flags.walkStepTimer += FixedDT;

                // repeat every 0.355f
                if (m_flags.walkStepTimer >= 0.355f)
                {
                    m_coord.GetSystem<AudioSystem>()->PlayRandomVariant("frogWalk", 3, 0.15f);
                    m_flags.walkStepTimer = 0.f;
                }
            }
            else
            {
                // Reset when stopping / airborne / unpossess
                m_flags.walkStepTimer = 0.f;
                m_flags.isWalking = false;
            }

            HandlePossessUnpossess();

            if (curr_ability == "Grapple")
            {
                m_frogTongue.UpdateOwner(m_player);
                m_frogTongue.UpdateIndicator(m_coord);
            }

            // Special Abilities --------------------------
            if (curr_ability == "DoubleJump") {
                m_flags.timeSinceLastDoubleJump += FixedDT;
                if (isGrounded) {
                    m_flags.canDoubleJump = true;
                }
            }

            if (m_flags.useAbility)
                UseSpecialAbility();
            else
            {
                if (m_flags.grappling)
                {
                    m_frogTongue.End(m_coord);
                    m_flags.grappling = false;
                }

                // reset glide flag when not gliding
                if (curr_ability == "Glide" && m_flags.isGliding) 
                {
                    m_flags.isGliding = false;
                }
            }

            // reset glide flag when grounded
            if (isGrounded && m_flags.isGliding) 
            {
                m_flags.isGliding = false;
            }

            prev_ability = curr_ability;
            m_flags.abilityKeyRelease = false;
        }

        /************************************************************************/
        /*!
        \brief
        Finds the current Player Entity.

        \param coord
        Reference to Coordinator.

        \return
        The ID of the current Player Entity.
        */
        /************************************************************************/
        Entity FindPlayer(Coordinator& coord);

        Entity GetPlayer() const noexcept { return m_player; }

        void LeftAndRightMovement(Vec2& vel, PlayerController const&) const;
        void Jump(Transform& t, Vec2& vel, PlayerController pcont, bool isGrounded);

        PlayerFlags& GetPlayerFlags();
        void ResetFlags() { m_flags = PlayerFlags{}; }
        void EnableDoubleJumpFlag() { m_flags.canDoubleJump = true; }

        // Store movement abilities as void pointers
        // std::function jump;
        // std::function special -> e.g double_jump, glide, grapple
        // 


        /************************************************************************/
        /*!
        \brief
        Gets refrence to FrogTongue which controls the grapple ability

        \return
        Refrence to FrogTongue
        */
        /************************************************************************/
        FrogTongue& GetFrogTongue() {
            return m_frogTongue;
        };

        /************************************************************************/
        /*!
        \brief
        Gets refrence to Bounce which controls the bounce ability

        \return
        Refrence to Bounce
        */
        /************************************************************************/
        Bounce& GetBounce() {
            return m_bounce;
        };
    
        // Ability stuff
        void RegisterAbilities();
        bool UseSpecialAbility();
        bool AttachAbility();
        std::string curr_ability = "Glide";
        std::string prev_ability;

        std::unordered_map<std::string, Gameplay::Ability> m_abilities;

        // Required by ISystem base class.
        std::string GetName() const override { return "PlayerController"; }
        Signature GetSignature() const override { return m_systemSignature; }

        /************************************************************************/
        /*!
        \brief
        Return distance entity is to player

        \param point
        Entity to check distacne to player

        \return
        float of distance of object to player
        */
        /************************************************************************/
        float DistanceToPlayer(Entity point)
        {
            if (!m_coord.HasComponent<Transform>(point) || !m_coord.HasComponent<Transform>(m_player))
                return 0;

            return magnitude(m_coord.GetComponent<Transform>(m_player).pos, m_coord.GetComponent<Transform>(point).pos);
        }


        /************************************************************************/
        /*!
        \brief
        Return the player entity

        \return
        The ID of the current Player Entity.
        */
        /************************************************************************/
        Entity GetPlayer() { return m_player; }

        /************************************************************************/
        /*!
        \brief
        Respawns player to cloest checkpoint
        Fails if previous checkpoint does not exist
        */
        /************************************************************************/
        void RespawnPlayer();

        void Reset() { reset = true; }
        void PauseInputs() { m_pauseInputs = true; }

        /************************************************************************/
        /*!
        \brief
        Creates the hitbox that checks for close to grounded state
        */
        /************************************************************************/
        bool CloseToGrounded();

        bool loadBackMainMenu{ false };

    private:
        /************************************************************************/
        /*!
        \brief
        Handles logic when jump/use ability button is pressed
        
        \param event
        Input event of the button pressed
        */
        /************************************************************************/
        void HandleJumpAbilityInput(const InputEvent& event);

        /************************************************************************/
        /*!
        \brief
        Handles logic when trying to possess/unpossess
        */
        /************************************************************************/
        void HandlePossessUnpossess();

        /************************************************************************/
        /*!
        \brief
        Creates the hitbox that checks for close to grounded state
        */
        /************************************************************************/
        void CreateGroundedHitbox();


        /************************************************************************/
        /*!
        \brief
        Creates the hitbox that checks for close to grounded state
        */
        /************************************************************************/
        void UpdateCloseGroundedHitBox();



        Signature m_systemSignature{};
        EventDispatcher* m_dispatcher = nullptr;
        Coordinator& m_coord;

        Entity m_player;
        bool reset;
        bool m_pauseInputs = false;

        // Accumulates states
        PlayerFlags m_flags{};

        //Coyote time
        float m_timeSinceGrounded{}; //Time since last grounded
        const float m_groundedThreshhold = 0.15f; //Time to be considered grounded
        //float m_groundedTime{};
        //const float m_repeatJumpThreshhold = 0.1f;

        float m_timeSinceAbility{};

        FrogTongue m_frogTongue;
        Bounce m_bounce;

        Entity m_prevCheckpoint = INVALID_ENTITY; //Tracks the prev checkpoint touched
        Entity m_closeGroundedHitBox = INVALID_ENTITY; //Tracks the hitbox to considered close to grounded, (e.g. prevent float from being used too close to ground)
        float m_closeGroundedHitBoxHeightOffset = 25; //How many units to add to player col to be considered close to grounded

        //bool m_midAir{}; //Is the player midair (used to help track if player jumped)
        //If spacebar is pressed right before landing allow 
        //bool m_storedJump = false;
        //float m_storedJumpTimer{};

        //Time to consider the player "grounded" for jumping
        //Lenght of time to store a jump
        //const float m_storedJumpThreshold = 0.2f;

        EmitterID m_playerJumpEmitter{};
    };

}   // namespace NightSnack
