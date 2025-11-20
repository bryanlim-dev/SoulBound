/******************************************************************************
 * File:        ScriptInterface.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file declares class ScriptInterface which contain functions which can 
 * interact with the ECS architecture from the outside.
 * 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "../ImportExport.h"
#include "Core/ECS/Coordinator.h"
#include "ScriptTypes.h"

#include <string>

namespace NightSnack::Scripting {

    // Interface that ScriptAPI will use to interact with engine
    class CORE_API ScriptInterface {

    public:
        ScriptInterface(Coordinator& coord) : m_coord{ coord } {}
        ~ScriptInterface() = default;

#pragma region Transform
        // Transform operations
        Trans GetTransform(Entity e);
        void SetTransform(Entity e, Trans& transform);

        // Individual getters/setters
        vec3 GetPosition(Entity e);
        void SetPosition(Entity e, vec3 pos);

        float GetRotation(Entity e);
        void SetRotation(Entity e, float rotation);

        vec2 GetScale(Entity e);
        void SetScale(Entity e, vec2 scale);

        LayerType GetLayer(Entity e);
        void SetLayer(Entity e, LayerType lay);
#pragma endregion

#pragma region Rigidbody
#pragma region Force
        // Force operations
        Forc GetForce(Entity e);
        void SetForce(Entity e, Forc& forc);

        // Individual getters/setters
        vec2 GetForceDirection(Entity e);
        void SetForceDirection(Entity e, vec2 dir);

        float GetForceMagnitude(Entity e);
        void SetForceMagnitude(Entity e, float mag);

        float GetForceLifetime(Entity e);
        void SetForceLifetime(Entity e, float life);

        bool GetForceActive(Entity e);
        void SetForceActive(Entity e, bool active);
#pragma endregion //Force

        RigidBod GetRigidBody(Entity e);
        void SetRigidBody(Entity e, RigidBod& bod);

        vec2 GetVelocity(Entity e);
        void SetVelocity(Entity e, vec2 vel);

        vec2 GetAcceleration(Entity e);
        void SetAcceleration(Entity e, vec2 accel);

        float GetInvMass(Entity e);
        void SetInvMass(Entity e, float invmas);

        float GetInvInertia(Entity e);
        void SetInvInertia(Entity e, float invInertia);

        float GetRestitution(Entity e);
        void SetRestitution(Entity e, float restitution);

        float GetGravityMult(Entity e);
        void SetGravityMult(Entity e, float gravMul);

        bool GetUseGravity(Entity e);
        void SetUseGravity(Entity e, bool useGravity);

        bool GetUseRotate(Entity e);
        void SetUseRotate(Entity e, bool useRotate);
#pragma endregion //Rigidbody
        
#pragma region Collider
        Collide GetCollider(Entity e);
        void SetCollider(Entity e, Collide col);

        ShapeType GetColliderShape(Entity e);
        void SetColliderShape(Entity e, ShapeType shap);

        ColliderType GetColliderType(Entity e);
        void SetColliderType(Entity e, ColliderType typ);

        bool GetColliderStatic(Entity e);
        void SetColliderStatic(Entity e, bool isStatic);

        float GetColliderHeight(Entity e);
        void SetColliderHeight(Entity e, float height);

        float GetColliderWidth(Entity e);
        void SetColliderWidth(Entity e, float width);

        float GetColliderRadius(Entity e);
        void SetColliderRadius(Entity e, float radius);

        bool GetColliderGrounded(Entity e);
        void SetColliderGrounded(Entity e, bool grounded);

        bool GetColliderColliding(Entity e);
        void SetColliderColliding(Entity e, bool colliding);
#pragma endregion

#pragma region Color
        vec4 GetColorRGBA(Entity e);
        void SetColorRGBA(Entity e, vec4 rgba);
#pragma endregion

#pragma region Renderable
        Renderabl GetRenderable(Entity e);
        void SetRenderable(Entity e, Renderabl rend);

        std::string GetMeshName(Entity e);
        void SetMeshName(Entity e, std::string name);

        std::string GetMaterialName(Entity e);
        void SetMaterialName(Entity e, std::string name);

        bool GetIsWireframe(Entity e);
        void SetIsWireframe(Entity e, bool wire);
#pragma endregion

#pragma region Animator
        Animat GetAnimator(Entity e);
        void SetAnimator(Entity e, Animat rend);

        std::string GetAnimatorCurrentAnimationClip(Entity e);
        void SetAnimatorCurrentAnimationClip(Entity e, std::string name);

        std::string GetAnimatorSpriteSheetName(Entity e);
        void SetAnimatorSpriteSheetName(Entity e, std::string name);

        int GetAnimatorCurrentFrameIndex(Entity e);
        void SetAnimatorCurrentFrameIndex(Entity e, int index);

        float GetAnimatorCurrentTime(Entity e);
        void SetAnimatorCurrentTime(Entity e, float time);

        float GetAnimatorPlaybackSpeed(Entity e);
        void SetAnimatorPlaybackSpeed(Entity e, float spd);

        bool GetAnimatorIsPlaying(Entity e);
        void SetAnimatorIsPlaying(Entity e, bool playing);

        bool GetAnimatorIsLooping(Entity e);
        void SetAnimatorIsLooping(Entity e, bool loop);

        bool GetAnimatorLastFacingRight(Entity e);
        void SetAnimatorLastFacingRight(Entity e, bool right);

        vec2 GetAnimatorUVTopLeft(Entity e);
        void SetAnimatorUVTopLeft(Entity e, vec2 uv);

        vec2 GetAnimatorUVBottomRight(Entity e);
        void SetAnimatorUVBottomRight(Entity e, vec2 uv);

#pragma endregion


#pragma region Key/Inputs
        // Input
        bool IsKeyPressed(int key);
        bool IsKeyDown(int key);
        bool IsKeyReleased(int key);
#pragma endregion

        // Audio
        void PlaySound(const std::string& path);

        // Time
        float GetDeltaTime();

        // Logging
        void Log(const std::string& msg);

        //Create Entity
        Entity CreateEntityArchetype(std::string archeTypeName, vec3 pos);

    private:
        Coordinator& m_coord;
    };

} // namespace NightSnack::Scripting
