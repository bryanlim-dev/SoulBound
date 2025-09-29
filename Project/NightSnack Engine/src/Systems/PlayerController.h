#pragma once

#include "./ECS/Coordinator.h"
#include "./ECS/Component.h"
#include "./ECS/ISystem.h"
#include "./Systems/Input.h"

class PlayerControllerSystem : public ISystem {

public:
    PlayerControllerSystem(Coordinator& coord, EventDispatcher& disp) : m_dispatcher{ &disp } {
        // Physics components of interest;
        m_systemSignature.set(GetComponentTypeID<PlayerController>());
        m_systemSignature.set(GetComponentTypeID<Position>());
        m_systemSignature.set(GetComponentTypeID<Rotation>());
        m_systemSignature.set(GetComponentTypeID<Scale>());
        m_systemSignature.set(GetComponentTypeID<Velocity>());

        FindPlayer(coord);

        m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
            PlayerController const& pcont = coord.GetComponent<PlayerController>(m_player);
            Force& force = coord.ModifyComponent<Force>(m_player);
            Velocity& vel = coord.ModifyComponent<Velocity>(m_player);
            
            if (event.pressed == InputEvent::InputActionState::Pressed) {
                switch (event.inputAction) {
                case InputEvent::InputAction::PlayerMoveLeft:

                    force.fx = -pcont.moveSpeed;
                    vel.dx = vel.dx < -200.f ? -200.f : vel.dx;
                    break;

                case InputEvent::InputAction::PlayerMoveRight:

                    force.fx = pcont.moveSpeed;
                    vel.dx = vel.dx > 200.f ? 200.f : vel.dx;
                    break;

                case InputEvent::InputAction::PlayerJump:
                    if (coord.GetComponent<Collider>(m_player).isGrounded) {
                        coord.ModifyComponent<Collider>(m_player).isGrounded = false;
                        force.fy = pcont.jumpForce;
                    }
                    break;
                case InputEvent::InputAction::ChangePlayer:
                    m_playerChange = true;
                    break;

                }
            }
            if (event.pressed == InputEvent::InputActionState::Released) {
                switch (event.inputAction) {
                case InputEvent::InputAction::PlayerMoveLeft:
                    force.fx = 0.f;
                    break;

                case InputEvent::InputAction::PlayerMoveRight:
                    force.fx = 0.f;
                    break;
                }
            }

        });
    }

    void Update(float dt, Coordinator& coord) override {
        (void)dt;

        m_player = FindPlayer(coord);
    }

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

    std::string GetName() const override { return "PlayerController"; }
    Signature GetSignature() const override { return m_systemSignature; }

private:
    Signature m_systemSignature{};
    EventDispatcher* m_dispatcher = nullptr;

    Entity m_player;
    bool m_playerChange{ false };
};
