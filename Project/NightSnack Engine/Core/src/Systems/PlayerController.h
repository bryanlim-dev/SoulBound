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

    bool playerChange = false;
    // add more flags if needed
};

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
            static bool landed = true;

            if (m_player == INVALID_ENTITY) { return; }

            PlayerController const& pcont = m_coord.GetComponent<PlayerController>(m_player);
            Vec2& vel = m_coord.ModifyComponent<RigidBody>(m_player).vel;
            Transform& t = m_coord.ModifyComponent<Transform>(m_player);

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
            if (m_flags.jumpRequested && m_coord.GetComponent<Collider>(m_player).isGrounded) {
                vel.y = pcont.jumpSpeed;
                t.pos.y += 2.5f;
                m_coord.ModifyComponent<Collider>(m_player).isGrounded = false;
                m_coord.GetSystem<AudioSystem>()->PlaySFX("playerJump");
                landed = false;
            }
            m_flags.jumpRequested = false;

            if (!m_coord.GetComponent<Collider>(m_player).isGrounded) landed = false;

            if (!landed && m_coord.GetComponent<Collider>(m_player).isGrounded) {
                m_coord.GetSystem<AudioSystem>()->PlaySFX("playerHurt");
                landed = true;
            }
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
        Entity FindPlayer(Coordinator& coord) {
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

        Entity GetPlayer() const noexcept { return m_player; }

        void ResetFlags() {
            m_flags = PlayerFlags{};
        }

        // Required by ISystem base class.
        std::string GetName() const override { return "PlayerController"; }
        Signature GetSignature() const override { return m_systemSignature; }


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
    };

}   // namespace NightSnack
