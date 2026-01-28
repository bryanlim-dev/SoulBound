/******************************************************************************
 * File:        ScriptInterface.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan - (Primary, 30%) - Created base file and Transform/Input/Sound functions
                Clement Ang Yang Ying - (Secondary, 70%) - Added other component and entity functions, orgnized functions
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

#include <mono/metadata/object.h>
#include <string>


//Macro to help register component for GetComponentIn<X>
#define REGISTER_COMPONENT(T)                                    \
{                                                                \
    #T,                                                          \
    {                                                            \
        /* GetComponentInChildren */                             \
        [](Coordinator& c, Entity e) -> Entity                   \
        {                                                        \
            if (auto& comp = c.GetComponentInChild<T>(e))        \
                return comp->second;                             \
            return Entity{};                                     \
        },                                                       \
        /* GetComponentInParent */                               \
        [](Coordinator& c, Entity e) -> Entity                   \
        {                                                        \
            if (auto& comp = c.GetComponentInParent<T>(e))       \
                return comp->second;                             \
            return Entity{};                                     \
        }                                                        \
    }                                                            \
}

namespace NightSnack::Scripting {

    // Interface that ScriptAPI will use to interact with engine
    class CORE_API ScriptInterface {

        //Used for GetComponentIn<X>
        using ComponentQueryFn = std::function<Entity(Coordinator&, Entity)>;
        struct ComponentEntry
        {
            ComponentQueryFn getInChildren;
            ComponentQueryFn getInParent;
        };

    public:
        ScriptInterface(Coordinator& coord) : m_coord{ coord } {}
        ~ScriptInterface() = default;

#pragma region Transform
        // Transform operations

        //Gets Transform component
        Trans GetTransform(Entity e);
        //Sets Transform component
        void SetTransform(Entity e, Trans& transform);

        //Get Transform position of entity
        vec3 GetPosition(Entity e);
        //Sets Transform position of entity
        void SetPosition(Entity e, vec3 pos);

        //Gets Transform rotation of entity
        float GetRotation(Entity e);
        //Sets Transform rotation of entity
        void SetRotation(Entity e, float rotation);

        //Gets Transform scale of entity
        vec2 GetScale(Entity e);
        //Sets Transform scale of entity
        void SetScale(Entity e, vec2 scale);

        //Gets Transform UIBasePos of entity
        vec3 GetUIBasePos(Entity e);
        //Sets Transform UIBasePos of entity
        void SetUIBasePos(Entity e, vec3 pos);

        //Gets Transform UIBaseScale of entity
        vec2 GetUIBaseScale(Entity e);
        //Sets Transform UIBaseScale of entity
        void SetUIBaseScale(Entity e, vec2 scal);

        //Gets Transform layer of entity
        LayerType GetLayer(Entity e);
        //Sets Transform layer of entity
        void SetLayer(Entity e, LayerType lay);
#pragma endregion

#pragma region Rigidbody
#pragma region Force
        // Force operations
        //Gets Force of entity
        Forc GetForce(Entity e);
        //Sets Force of entity
        void SetForce(Entity e, Forc& forc);

        //Gets Force direction of entity
        vec2 GetForceDirection(Entity e);
        //Sets Force direction of entity
        void SetForceDirection(Entity e, vec2 dir);

        //Gets Force magnitude of entity
        float GetForceMagnitude(Entity e);
        //Sets Force magnitude of entity
        void SetForceMagnitude(Entity e, float mag);

        //Gets Force lifetime of entity
        float GetForceLifetime(Entity e);
        //Sets Force lifetime of entity
        void SetForceLifetime(Entity e, float life);

        //Gets Force is active of entity
        bool GetForceActive(Entity e);
        //Sets Force is active of entity
        void SetForceActive(Entity e, bool active);
#pragma endregion //Force

        //Gets Rigidbody of entity
        RigidBod GetRigidBody(Entity e);
        //Sets Rigidbody of entity
        void SetRigidBody(Entity e, RigidBod& bod);

        //Gets Rigidbody veloctiy of entity
        vec2 GetVelocity(Entity e);
        //Sets Rigidbody veloctiy of entity
        void SetVelocity(Entity e, vec2 vel);

        //Gets Rigidbody acceleration of entity
        vec2 GetAcceleration(Entity e);
        //Sets Rigidbody acceleration of entity
        void SetAcceleration(Entity e, vec2 accel);

        //Gets Rigidbody InvMass of entity
        float GetInvMass(Entity e);
        //Sets Rigidbody InvMass of entity
        void SetInvMass(Entity e, float invmas);

        //Gets Rigidbody InvInertia of entity
        float GetInvInertia(Entity e);
        //Sets Rigidbody InvInertia of entity
        void SetInvInertia(Entity e, float invInertia);

        //Gets Rigidbody Restitution of entity
        float GetRestitution(Entity e);
        //Sets Rigidbody Restitution of entity
        void SetRestitution(Entity e, float restitution);

        //Gets Rigidbody GravMult of entity
        float GetGravityMult(Entity e);
        //Sets Rigidbody GravMult of entity
        void SetGravityMult(Entity e, float gravMul);

        //Gets Rigidbody useGravity of entity
        bool GetUseGravity(Entity e);
        //Sets Rigidbody useGravity of entity
        void SetUseGravity(Entity e, bool useGravity);

        //Gets Rigidbody useRotate of entity
        bool GetUseRotate(Entity e);
        //Sets Rigidbody useRotate of entity
        void SetUseRotate(Entity e, bool useRotate);
#pragma endregion //Rigidbody
        
#pragma region Collider
        //Gets Collider of entity
        Collide GetCollider(Entity e);
        //Sets Collider of entity
        void SetCollider(Entity e, Collide col);

        //Gets Collider shape of entity
        ShapeType GetColliderShape(Entity e);
        //Sets Collider shape of entity
        void SetColliderShape(Entity e, ShapeType shap);

        //Gets Collider type of entity
        ColliderType GetColliderType(Entity e);
        //Sets Collider type of entity
        void SetColliderType(Entity e, ColliderType typ);

        //Gets Collider static of entity
        bool GetColliderStatic(Entity e);
        //Sets Collider static of entity
        void SetColliderStatic(Entity e, bool isStatic);

        //Gets Collider height of entity
        float GetColliderHeight(Entity e);
        //Sets Collider height of entity
        void SetColliderHeight(Entity e, float height);

        //Gets Collider width of entity
        float GetColliderWidth(Entity e);
        //Sets Collider width of entity
        void SetColliderWidth(Entity e, float width);

        //Gets Collider radius of entity
        float GetColliderRadius(Entity e);
        //Sets Collider radius of entity
        void SetColliderRadius(Entity e, float radius);

        //Gets Collider isGrounded of entity
        bool GetColliderGrounded(Entity e);
        //Sets Collider isGrounded of entity
        void SetColliderGrounded(Entity e, bool grounded);

        //Gets Collider isCOlliding of entity
        bool GetColliderColliding(Entity e);
        //Sets Collider isCOlliding of entity
        void SetColliderColliding(Entity e, bool colliding);
#pragma endregion

#pragma region Color
        //Gets Color of entity
        vec4 GetColorRGBA(Entity e);
        //Sets Color of entity
        void SetColorRGBA(Entity e, vec4 rgba);
#pragma endregion

#pragma region Renderable
        //Gets Renderable of entity
        Renderabl GetRenderable(Entity e);
        //Sets Renderable of entity
        void SetRenderable(Entity e, Renderabl rend);

        //Gets Renderable mesh name of entity
        std::string GetMeshName(Entity e);
        //Sets Renderable mesh name of entity
        void SetMeshName(Entity e, std::string name);

        //Gets Renderable maertial name of entity
        std::string GetMaterialName(Entity e);
        //Sets Renderable maertial name of entity
        void SetMaterialName(Entity e, std::string name);

        //Gets Renderable isWireframe of entity
        bool GetIsWireframe(Entity e);
        //Sets Renderable isWireframe of entity
        void SetIsWireframe(Entity e, bool wire);
#pragma endregion


#pragma region Animator
        //Gets Animator of entity
        Animat GetAnimator(Entity e);
        //Sets Animator of entity
        void SetAnimator(Entity e, Animat rend);

        //Gets Animator current animation clip of entity
        std::string GetAnimatorCurrentAnimationClip(Entity e);
        //Sets Animator current animation clip of entity
        void SetAnimatorCurrentAnimationClip(Entity e, std::string name);

        //Gets Animator current animation spritesheet of entity
        std::string GetAnimatorSpriteSheetName(Entity e);
        //Sets Animator current animation spritesheet of entity
        void SetAnimatorSpriteSheetName(Entity e, std::string name);

        //Gets Animator current animation frameIndex of entity
        int GetAnimatorCurrentFrameIndex(Entity e);
        //Sets Animator current animation frameIndex of entity
        void SetAnimatorCurrentFrameIndex(Entity e, int index);

        //Gets Animator current animation time of entity
        float GetAnimatorCurrentTime(Entity e);
        //Sets Animator current animation time of entity
        void SetAnimatorCurrentTime(Entity e, float time);

        //Gets Animator current animation playbackSpeed of entity
        float GetAnimatorPlaybackSpeed(Entity e);
        //Sets Animator current animation playbackSpeed of entity
        void SetAnimatorPlaybackSpeed(Entity e, float spd);

        //Gets Animator current animation playing of entity
        bool GetAnimatorIsPlaying(Entity e);
        //Sets Animator current animation playing of entity
        void SetAnimatorIsPlaying(Entity e, bool playing);

        //Gets Animator current animation looping of entity
        bool GetAnimatorIsLooping(Entity e);
        //Sets Animator current animation looping of entity
        void SetAnimatorIsLooping(Entity e, bool loop);

        //Gets Animator current animation facingRight of entity
        bool GetAnimatorLastFacingRight(Entity e);
        //Sets Animator current animation facingRight of entity
        void SetAnimatorLastFacingRight(Entity e, bool right);

        //Gets Animator current animation UVTopLeft of entity
        vec2 GetAnimatorUVTopLeft(Entity e);
        //Sets Animator current animation UVTopLeft of entity
        void SetAnimatorUVTopLeft(Entity e, vec2 uv);

        //Gets Animator current animation UVBottomRight of entity
        vec2 GetAnimatorUVBottomRight(Entity e);
        //Sets Animator current animation UVBottomRight of entity
        void SetAnimatorUVBottomRight(Entity e, vec2 uv);


#pragma endregion

#pragma region Font
        //Gets Font of entity
        Fon GetFont(Entity e);
        //Sets Font of entity
        void SetFont(Entity e, Fon rend);

        //Gets Font type of entity
        std::string GetFontType(Entity e);
        //Sets Font type of entity
        void SetFontType(Entity e, std::string name);

        //Gets Font text of entity
        std::string GetFontText(Entity e);
        //Sets Font text of entity
        void SetFontText(Entity e, std::string txt);

        //Gets Font size of entity
        float GetFontSize(Entity e);
        //Sets Font size of entity
        void SetFontSize(Entity e, float sz);

        //Gets Font color of entity
        vec3 GetFontTextColor(Entity e);
        //Sets Font color of entity
        void SetFontTextColor(Entity e, vec3 col);

        //Gets Font offsetPos of entity
        vec3 GetFontOffsetPosition(Entity e);
        //Sets Font offsetPos of entity
        void SetFontOffsetPosition(Entity e, vec3 pos);

#pragma endregion

#pragma region Camera
        //Gets Camera of entity
        Cam GetCamera(Entity e);
        //Sets Camera of entity
        void SetCamera(Entity e, Cam cam);

        //Gets Camera isMain of entity
        bool GetCameraIsMain(Entity e);
        //Sets Camera isMain of entity
        void SetCameraIsMain(Entity e, bool main);

        //Gets Camera zoom of entity
        float GetCameraZoom(Entity e);
        //Sets Camera zoom of entity
        void SetCameraZoom(Entity e, float main);
        
        //Gets Camera width of entity
        float GetCameraWidth(Entity e);
        //Sets Camera width of entity
        void SetCameraWidth(Entity e, float main);

        //Gets Camera height of entity
        float GetCameraHeight(Entity e);
        //Sets Camera height of entity
        void SetCameraHeight(Entity e, float main);
#pragma endregion

#pragma region Audio
		//Gets Audio of entity
        Audi GetAudio(Entity e);
		//Sets Audio of entity
		void SetAudio(Entity e, Audi aud);

        //Gets Audio audio key
		std::string GetAudioKey(Entity e);
        //Sets Audio audio key
		void SetAudioKey(Entity e, std::string key);

		//Gets Audio volume
		float GetAudioVolume(Entity e);
		//Sets Audio volume
		void SetAudioVolume(Entity e, float vol);

        //Gets Audio looping
        bool GetAudioLoop(Entity e);
        //Sets Audio looping
        void SetAudioLoop(Entity e, bool looping);

        //Gets Audio playOnStart
        bool GetAudioPlayOnStart(Entity e);
        //Sets Audio playOnStart
        void SetAudioPlayOnStart(Entity e, bool playOnStart);

        //Gets Audio isPlaying
        bool GetAudioIsPlaying(Entity e);
        //Sets Audio isPlaying
        void SetAudioIsPlaying(Entity e, bool isPlaying);


#pragma endregion


#pragma region Scripting

        //Check does entity have a script
        bool HasScript(Entity e, std::string str);
		//Creates script for entity
        void CreateScript(Entity e, std::string str);
		//Delets script for entity
        void DeleteScript(Entity e, std::string str);
		//Gets script ptr for entity
        MonoObject* GetScript(Entity e, std::string str);
#pragma endregion



#pragma region Key/Inputs
        //Check if key pressed
        bool IsKeyPressed(int key);
        //Check if key down
        bool IsKeyDown(int key);
        //Check if key released
        bool IsKeyReleased(int key);
#pragma endregion

        Entity GetComponentInChildren(Entity entity, std::string componentName);
        MonoObject* GetScriptComponentInChildren(Entity entity, std::string scriptName);

        Entity GetComponentInParent(Entity entity, std::string componentName);
        MonoObject* GetScriptComponentInParent(Entity entity, std::string scriptName);

        // Audio play sound
        void PlaySound(const std::string& path);

        // Time get DT
        float GetDeltaTime();

        // Logging output to console
        void Log(const std::string& msg);

        //Create Entity
        Entity CreateEntityArchetype(std::string archeTypeName, vec3 pos);

        //Gets player ID
        Entity GetPlayerID();

        //Loads level based on a name
        void LoadLevel(std::string lvlName);

        //Restarts current level
        void RestartLevel();

    private:
        //coordiator refrence
        Coordinator& m_coord;

        static inline const std::unordered_map<std::string, ComponentEntry>
            componentRegistry =
        {
            REGISTER_COMPONENT(Transform),
            REGISTER_COMPONENT(RigidBody),
            REGISTER_COMPONENT(Collider),
            REGISTER_COMPONENT(Color),
            REGISTER_COMPONENT(Renderable),
            REGISTER_COMPONENT(Animator),
            REGISTER_COMPONENT(Font),
            REGISTER_COMPONENT(Audio)
        };

    };

} // namespace NightSnack::Scripting
