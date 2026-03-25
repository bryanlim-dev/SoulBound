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
        PlayerControllerSystem(Coordinator& coord, EventDispatcher& disp);

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
            if (m_abilityBox == INVALID_ENTITY || m_abilityKey == INVALID_ENTITY
                || m_coord.GetComponent<Transform>(m_abilityBox).name != "UI-AbilityBox"
                || m_coord.GetComponent<Transform>(m_abilityKey).name != "UI-AbilityKey") {
                FindAbilityBox(m_coord);
            }

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
                // Reset stored entities
                m_closeGroundedHitBox = INVALID_ENTITY;
                m_abilityBox = INVALID_ENTITY;
                m_abilityKey = INVALID_ENTITY;
                //m_player = INVALID_ENTITY;
                //m_prevCheckpoint = INVALID_ENTITY;

                AttachAbility();
                m_flags = PlayerFlags{};

                //False as the scene manager should handle cleanup deletion anyways
                m_bounce.Reset(m_coord, false);
                m_frogTongue.Reset(m_coord, false);

                reset = false;
                m_pauseInputs = false;

                m_player = FindPlayer(m_coord);

                CreateGroundedHitbox();
            }

            if (m_pauseInputs) return;

            UpdateCloseGroundedHitBox();

            PlayerController const& pcont = m_coord.GetComponent<PlayerController>(m_player);
            Vec2& vel = m_coord.ModifyComponent<RigidBody>(m_player).vel;
            Transform& t = m_coord.ModifyComponent<Transform>(m_player);
            Collider& c = m_coord.ModifyComponent<Collider>(m_player);
            Renderable& r = m_coord.ModifyComponent<Renderable>(m_player);

            //
            static float time = 0.f;
            //Only sure time y == 0 when landed
            //Other is maybe peak of jump but that wont trigger consistantly and more importantly 
            //dosnt long enough to trigger grounded and just jumped dosnt matter in midair
            if (vel.y == 0.f) {
                time += FixedDT;
            }
            else {
                time = 0.f;
            }
            if (time >= 0.15f) {
                c.isGrounded = true;
                m_timeSinceGrounded = 0;
                time = 0.f;
            }

            m_timeSinceGrounded += FixedDT;
            
            if (c.isGrounded || CloseToGrounded())
                m_flags.justJumped = false;


            isGrounded = m_flags.isGrounded = c.isGrounded || m_timeSinceGrounded < m_groundedThreshhold;

            // Basic movement -----------------------------
            if (!m_flags.disableMovement)
            {
                LeftAndRightMovement(vel, pcont);
                Jump(t, vel, pcont, isGrounded);
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



			// reset glide when grounded
            if (isGrounded)
            {
                if (m_flags.isGliding)
                    m_flags.isGliding = false;

                if (curr_ability == "Glide")
                {
                    r.materialName = "SpiritMove_4_1";
                }
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

            if (m_flags.useAbility) {
                UseSpecialAbility();
                ActivateAbilityBox();
            }
            else
            {
                DeactivateAbilityBox();
                if (m_flags.grappling)
                {
                    m_frogTongue.End(m_coord);
                    m_flags.grappling = false;
                }

                // reset glide flag when not gliding
                if (curr_ability == "Glide") 
                {
                    if (m_flags.isGliding)
                    {
                        m_flags.isGliding = false;
                        r.materialName = "SpiritMove_4_1";
                    }
                    else if (curr_ability == "Glide")
                    {
                        auto& body = m_coord.GetComponent<RigidBody>(m_player);
                        if (body.vel.y < 0.f)
                            r.materialName = "PlayerFall_4_1";
                        else
                            r.materialName = "SpiritMove_4_1";
                    }
                }
            }

            // reset glide flag when grounded
            if (isGrounded)
            {
                if (m_flags.isGliding)
                    m_flags.isGliding = false;

                if (curr_ability == "Glide")
                {
                    r.materialName = "SpiritMove_4_1";
                }

                if (curr_ability == "Grapple" && !m_flags.isWalking)
                {
                    r.materialName = "FrogIdle_4_1";
                }
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
        void FindAbilityBox(Coordinator& coord);

        Entity GetPlayer() const noexcept { return m_player; }
		void PrintPause() const noexcept { std::cout << (m_pauseInputs ? "Paused\n" : "Unpaused\n"); }

        void LeftAndRightMovement(Vec2& vel, PlayerController const&);
        void Jump(Transform& t, Vec2& vel, PlayerController pcont, bool grounded);

        PlayerFlags& GetPlayerFlags();
        void ResetFlags() { m_flags = PlayerFlags{}; UnpauseInputs(); }
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
        int GetAbilityNumber() const;
        std::string curr_ability = "Glide";
        std::string prev_ability;

        std::unordered_map<std::string, Gameplay::Ability> m_abilities;
        std::unordered_map<std::string, std::string> m_abilityDefaultTextures; //map of textures for each ability, key is the ability name

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
        void UnpauseInputs() { m_pauseInputs = false; }

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

        void ActivateAbilityBox();
        void DeactivateAbilityBox();

        Signature m_systemSignature{};
        EventDispatcher* m_dispatcher = nullptr;
        Coordinator& m_coord;

        Entity m_player = INVALID_ENTITY;
        Entity m_abilityBox = INVALID_ENTITY;
        Entity m_abilityKey = INVALID_ENTITY;
        bool reset;
        bool m_pauseInputs = false;
		bool isGrounded = false;

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
