/******************************************************************************
 * File:        PlayerController.h
 * Project:     NightSnack
 * Author(s):   Chewn Thing Kwan
 *
 * Description:
 * Controls the Player Entity and sets their Velocity component when key inputs
 * are detected.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "./ECS/Coordinator.h"
#include "./ECS/Component.h"
#include "./ECS/ISystem.h"
#include "./Systems/Input.h"

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
    PlayerControllerSystem(Coordinator& coord, EventDispatcher& disp) : m_dispatcher{ &disp } {
        // Physics components of interest;
        m_systemSignature.set(GetComponentTypeID<PlayerController>());
        m_systemSignature.set(GetComponentTypeID<Position>());
        m_systemSignature.set(GetComponentTypeID<Rotation>());
        m_systemSignature.set(GetComponentTypeID<Scale>());
        m_systemSignature.set(GetComponentTypeID<Velocity>());

        m_player = FindPlayer(coord);
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
    void Update(float dt, Coordinator& coord) override {
        (void)dt;
        
        m_player = FindPlayer(coord);

        PlayerController const& pcont = coord.GetComponent<PlayerController>(m_player);
        Velocity& vel = coord.ModifyComponent<Velocity>(m_player);

        if (coord.GetSystem<InputSystem>()->IsKeyDown(InputSystem::RIGHT_ARROW) && !coord.GetComponent<Collider>(m_player).colliding) {
            vel.dx = pcont.moveSpeed;
        }
        else if (coord.GetSystem<InputSystem>()->IsKeyDown(InputSystem::LEFT_ARROW) && !coord.GetComponent<Collider>(m_player).colliding) {
            vel.dx = -pcont.moveSpeed;
        }
        if ((coord.GetSystem<InputSystem>()->IsKeyDown(InputSystem::UP_ARROW))) {
            if (coord.GetComponent<Collider>(m_player).isGrounded) {
                vel.dy = pcont.jumpSpeed;
                coord.ModifyComponent<Collider>(m_player).isGrounded = false;
            }
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
        playerSig.set(GetComponentTypeID<Position>());
        playerSig.set(GetComponentTypeID<Rotation>());
        playerSig.set(GetComponentTypeID<Scale>());

        playerSig.set(GetComponentTypeID<Force>());
        playerSig.set(GetComponentTypeID<Acceleration>());
        playerSig.set(GetComponentTypeID<Velocity>());
        playerSig.set(GetComponentTypeID<Mass>());
        playerSig.set(GetComponentTypeID<Collider>());

        // Iterate through all living Entities in the COORDINATOR instead
        // Since the GraphicsSystem will not have a Camera Entity
        for (Entity const& e : coord.GetLivingEntities()) {
            if ((coord.GetEntitySignature(e) & playerSig) == playerSig) { // This camera Entity is the main camera
                return e;
            }
        }
        std::cerr << "Error: No Player Entity found in scene!\n";
        return INVALID_ENTITY;
    }

    // Required by ISystem base class.
    std::string GetName() const override { return "PlayerController"; }
    Signature GetSignature() const override { return m_systemSignature; }

private:
    Signature m_systemSignature{};
    EventDispatcher* m_dispatcher = nullptr;

    Entity m_player;
    bool m_playerChange{ false };
};
