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

            m_player = FindPlayer(coord);

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
                        if (event.pressed == InputEvent::InputActionState::Pressed ||
                            event.pressed == InputEvent::InputActionState::Repeat) {
                            m_flags.jumpRequested = true;
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
        void Update(int steps) override {
            (void)steps;

            m_player = FindPlayer(m_coord);

            PlayerController const& pcont = m_coord.GetComponent<PlayerController>(m_player);
            Vec2& vel = m_coord.ModifyComponent<RigidBody>(m_player).vel;

            if ((m_flags.movingRight && !m_flags.movingLeft) && !m_coord.GetComponent<Collider>(m_player).colliding) {
                vel.x = pcont.moveSpeed;
            }
            else if ((m_flags.movingLeft && !m_flags.movingRight) && !m_coord.GetComponent<Collider>(m_player).colliding) {
                vel.x = -pcont.moveSpeed;
            }
            if (m_flags.jumpRequested && m_coord.GetComponent<Collider>(m_player).isGrounded) {
                vel.y = pcont.jumpSpeed;
                m_coord.ModifyComponent<Collider>(m_player).isGrounded = false;
            }
            m_flags.jumpRequested = false;
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

            playerSig.set(GetComponentTypeID<Force>());
            playerSig.set(GetComponentTypeID<RigidBody>());
            playerSig.set(GetComponentTypeID<Collider>());

            // Iterate through all living Entities in the COORDINATOR instead
            // Since the GraphicsSystem will not have a Camera Entity
            for (Entity const& e : coord.GetLivingEntities()) {
                if ((coord.GetEntitySignature(e) & playerSig) == playerSig) { // This camera Entity is the main camera
                    return e;
                }
            }
            //std::cerr << "Error: No Player Entity found in scene!\n";
            return INVALID_ENTITY;
        }

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
