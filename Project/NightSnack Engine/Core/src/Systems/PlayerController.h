/******************************************************************************
 * File:        PlayerController.h
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * Controls the Player Entity and sets their Velocity component when key inputs
 * are detected.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/Input.h"
#include "Systems/AudioSystem.h"

struct PlayerFlags {
    bool movingLeft = false;
    bool movingRight = false;

    bool jumpRequested = false;
    bool justJumped{};

    bool playerChange = false;
    bool isGrounded = false;

    bool isPossessing = false;
    bool useAbility = false;

    bool canDoubleJump = true;
    // add more flags if needed
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

            m_player = 1;

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
                    case InputEvent::InputAction::PlayerSpecialAbility:
                        if (event.pressed == InputEvent::InputActionState::Pressed ||
                            event.pressed == InputEvent::InputActionState::Repeat) {
                            m_flags.useAbility = true;
                        }
                        else {
                            m_flags.useAbility = false;
                        }
                        break;
                    case InputEvent::InputAction::PlayerJump:
                        if (event.pressed == InputEvent::InputActionState::Pressed) {
                            m_flags.jumpRequested = true;
                        }
                        else {
                            m_flags.jumpRequested = false;
                        }
                        break;
                    }
                });
            RegisterAbilities();
            AttachAbility();
            prev_ability = curr_ability;
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

            if (m_coord.IsPaused()) return;

            m_player = FindPlayer(m_coord);
            //static bool landed = true;

            if (m_player == INVALID_ENTITY) { return; }

            PlayerController const& pcont = m_coord.GetComponent<PlayerController>(m_player);
            Vec2& vel = m_coord.ModifyComponent<RigidBody>(m_player).vel;
            Transform& t = m_coord.ModifyComponent<Transform>(m_player);

            bool isGrounded = m_coord.GetComponent<Collider>(m_player).isGrounded;

            // Basic movement -----------------------------
            LeftAndRightMovement(vel, pcont);
            Jump(t, vel, pcont, isGrounded);

            if (isGrounded) m_flags.canDoubleJump = true;
            // Special Abilities --------------------------
            if (curr_ability != prev_ability)
                if (!AttachAbility()) std::cerr << "Failed to attach ability!\n";
            if (m_flags.useAbility)
                UseSpecialAbility();
                //if (!UseSpecialAbility()) std::cerr << "Failed to use ability!\n";

            prev_ability = curr_ability;
            m_timeSinceAbility += FixedDT;
            if (m_timeSinceAbility >= 1.0f) m_timeSinceAbility = 0.f;

            //HoldJump(vel, isGrounded);

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

        void ResetFlags() { m_flags = PlayerFlags{}; }

        // Store movement abilities as void pointers
        // std::function jump;
        // std::function special -> e.g double_jump, glide, grapple
        // 
    
        // Ability stuff
        void RegisterAbilities();
        bool UseSpecialAbility();
        bool AttachAbility();
        std::string curr_ability;
        std::string prev_ability;

        std::unordered_map<std::string, Gameplay::Ability> m_abilities;

        // Required by ISystem base class.
        std::string GetName() const override { return "PlayerController"; }
        Signature GetSignature() const override { return m_systemSignature; }

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

    private:
        Signature m_systemSignature{};
        EventDispatcher* m_dispatcher = nullptr;
        Coordinator& m_coord;

        Entity m_player;

        // Accumulates states
        PlayerFlags m_flags{};

        //Cyote time
        float m_timeSinceGrounded{}; //Time since last grounded
        const float m_groundedThreshhold = 0.2f;

        float m_timeSinceAbility{};

        //bool m_midAir{}; //Is the player midair (used to help track if player jumped)
        //If spacebar is pressed right before landing allow 
        //bool m_storedJump = false;
        //float m_storedJumpTimer{};

        //Time to consider the player "grounded" for jumping
        //Lenght of time to store a jump
        //const float m_storedJumpThreshold = 0.2f;
    };

}   // namespace NightSnack
