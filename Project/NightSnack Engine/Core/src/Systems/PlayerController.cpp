#include "PlayerController.h"

namespace NightSnack {

    void PlayerControllerSystem::RegisterAbilities() {

        std::string abilityName = "Glide";
        Gameplay::Ability GlideAbility(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;

            Vec2& vel = c.ModifyComponent<RigidBody>(target).vel;
            if (vel.y <= -00.f) vel.y = -100.f;
            });
        m_abilities.insert({ abilityName, GlideAbility });

        abilityName = "DoubleJump";
        Gameplay::Ability DoubleJumpAbility(abilityName, [](Coordinator& c, Entity target, PlayerFlags& m_flags) {
            //if (target == INVALID_ENTITY) return;
            if (!c.HasComponent<RigidBody>(target)) return;
            if (!c.HasComponent<PlayerController>(target)) return;
            if (!m_flags.canDoubleJump) return;

            Vec2& vel = c.ModifyComponent<RigidBody>(target).vel;
            vel.y = c.GetComponent<PlayerController>(target).jumpSpeed;

            m_flags.canDoubleJump = false;

        });
        m_abilities.insert({ abilityName, DoubleJumpAbility });

    }

    bool PlayerControllerSystem::AttachAbility() {
        curr_ability = "DoubleJump";
        return true;
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

    void PlayerControllerSystem::Jump(Transform& t, Vec2& vel, PlayerController pcont, bool isGrounded) {

        bool jump = false;
        m_timeSinceGrounded += FixedDT;
        if (m_timeSinceGrounded > m_groundedThreshhold)
            m_flags.justJumped = false;
        if (isGrounded)
            m_timeSinceGrounded = 0;

        if (m_flags.jumpRequested) {
            // allow jump if isgrounded
            if (isGrounded) {
                jump = true;
            }
            // allow jump if pressed right after being off ground
            if (!isGrounded && m_timeSinceGrounded < m_groundedThreshhold && !m_flags.justJumped)
                jump = true;
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


        if (jump) {
            vel.y = pcont.jumpSpeed;
            t.pos.y += 0.5f;
            m_coord.ModifyComponent<Collider>(m_player).isGrounded = false;
            m_coord.GetSystem<AudioSystem>()->PlaySFX("playerJump");

            //m_storedJump = false;
            m_flags.justJumped = true;
        }
    }
}