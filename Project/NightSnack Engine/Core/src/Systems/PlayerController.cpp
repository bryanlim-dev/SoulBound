#include "PlayerController.h"

namespace NightSnack {

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

    void PlayerControllerSystem::LeftAndRightMovement(Vec2& vel, PlayerController const& pcont) {
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
    }

    void PlayerControllerSystem::HoldJump(Vec2& vel, bool isGrounded) {
        static float time = 0.5f;
        static float hang = 50.f;
        // Hold jump to go higher
        if (m_flags.jumpHold && !isGrounded && m_flags.canHang)
        {
            if (time > 0.f && hang > 0.f) {
                vel.y += hang * time;
                time -= FixedDT;

                m_flags.jumpHold = false;
            }
        }
        else {
            time = 0.5f;
            hang = 100.f;
        }
        std::cout << "hang: " << hang << ", time: " << time << std::endl;

        if (isGrounded) {
            m_flags.canHang = true;
        }
    }

    void PlayerControllerSystem::Jump(Transform& t, Vec2& vel, PlayerController pcont, bool isGrounded) {

        bool jump = false;
        m_timeSinceGrounded += FixedDT;
        if (m_timeSinceGrounded > m_groundedThreshhold)
            m_justJumped = false;
        if (isGrounded)
            m_timeSinceGrounded = 0;

        if (m_flags.jumpRequested) {
            // allow jump if isgrounded
            if (isGrounded) {
                jump = true;
            }
            // allow jump if pressed right after being off ground
            if (!isGrounded && m_timeSinceGrounded < m_groundedThreshhold && !m_justJumped)
                jump = true;
        }

        // allow jump if pressed right before being grounded
        if (m_flags.jumpRequested && !jump)
            m_storedJump = true;
        if (m_storedJump) {
            m_storedJumpTimer += FixedDT;
            if (m_storedJumpTimer > m_storedJumpThreshold) {
                m_storedJump = false;
                m_storedJumpTimer = 0.f;
            }
        }
        if (m_storedJump && isGrounded)
            jump = true;


        if (jump) {
            vel.y = pcont.jumpSpeed;
            t.pos.y += 0.5f;
            m_coord.ModifyComponent<Collider>(m_player).isGrounded = false;
            m_coord.GetSystem<AudioSystem>()->PlaySFX("playerJump");

            m_storedJump = false;
            m_justJumped = true;
        }
    }
}