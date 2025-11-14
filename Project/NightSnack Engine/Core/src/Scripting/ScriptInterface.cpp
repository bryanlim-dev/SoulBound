/******************************************************************************
 * File:        ScriptInterface.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file defines functions convert native component types to types visible
 * to C# Scripting API.
 * This is the furthest that the ECS Coordinator will be able to reach.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "ScriptInterface.h"

#include "./Systems/AudioSystem.h"
#include "./Systems/Application/Application.h"
#include "./Systems/Application/Input.h"
#include "../Systems/AllSystems.h"

#include <iostream>

namespace NightSnack::Scripting {

    // --------------------------------------------------------------
    // Transform operations
#pragma region Transform
    Trans ScriptInterface::GetTransform(Entity e) {

        Transform t = m_coord.GetComponent<Transform>(e);
        Trans result;

        result.pos = vec3(t.pos.x, t.pos.y, t.pos.z);
        result.rot = t.rot;
        result.scale = vec2(t.scale.x, t.scale.y);
        result.layer = t.layer;

        return result;
    }
    void ScriptInterface::SetTransform(Entity e, Trans& t) {

        Transform& trans = m_coord.ModifyComponent<Transform>(e);

        trans.pos = Vec3(t.pos.x, t.pos.y, t.pos.z);
        trans.rot = t.rot;
        trans.scale = Vec2(t.scale.x, t.scale.y);
        trans.layer = t.layer;
    }
    // Position ----
    vec3 ScriptInterface::GetPosition(Entity e) {
        Vec3 position = m_coord.GetComponent<Transform>(e).pos;
        vec3 result = vec3(position.x, position.y, position.z);
        return result;
    }
    void ScriptInterface::SetPosition(Entity e, vec3 pos) {
        Vec3& position = m_coord.ModifyComponent<Transform>(e).pos;
        position = Vec3(pos.x, pos.y, pos.z);
    }

    // Rotation ----
    float ScriptInterface::GetRotation(Entity e) {
        return m_coord.GetComponent<Transform>(e).rot;
    }
    void ScriptInterface::SetRotation(Entity e, float rotate) {
        m_coord.ModifyComponent<Transform>(e).rot = rotate;
    }

    // Scale -------
    vec2 ScriptInterface::GetScale(Entity e) {
        Vec2 scale = m_coord.GetComponent<Transform>(e).scale;
        return vec2(scale.x, scale.y);
    }
    void ScriptInterface::SetScale(Entity e, vec2 scal) {
        Vec2& scale = m_coord.ModifyComponent<Transform>(e).scale;
        scale = Vec2(scal.x, scal.y);
    }

    // Layer -------
    LayerType ScriptInterface::GetLayer(Entity e) {
        LayerType layer = m_coord.GetComponent<Transform>(e).layer;
        return layer;
    }
    void ScriptInterface::SetLayer(Entity e, LayerType lay) {
        LayerType& layer = m_coord.ModifyComponent<Transform>(e).layer;
        layer = lay;
    }
#pragma endregion

    // --------------------------------------------------------------
    // Force Inputs
#pragma region Force
    Forc ScriptInterface::GetForce(Entity e) {

        Force t = m_coord.GetComponent<Force>(e);
        Forc result;

        result.direction = vec2(t.direction.x, t.direction.y);
        result.magnitude = t.magnitude;
        result.isActive = t.isActive;
        result.lifetime = t.lifetime;

        return result;
    }
    void ScriptInterface::SetForce(Entity e, Forc& t) {

        Force& force = m_coord.ModifyComponent<Force>(e);

        force.direction = Vec2(t.direction.x, t.direction.y);
        force.magnitude = t.magnitude;
        force.isActive = t.isActive;
        force.lifetime = t.lifetime;
    }

    // Direction ----
    vec2 ScriptInterface::GetForceDirection(Entity e) {
        Vec2 direction = m_coord.GetComponent<Force>(e).direction;
        vec2 result = vec2(direction.x, direction.y);
        return result;
    }
    void ScriptInterface::SetForceDirection(Entity e, vec2 pos) {
        Vec2& direction = m_coord.ModifyComponent<Force>(e).direction;
        direction = Vec3(pos.x, pos.y);
    }

    // Magnitude ----
    float ScriptInterface::GetForceMagnitude(Entity e)
    {
        float magnitude = m_coord.GetComponent<Force>(e).magnitude;
        return magnitude;
    }
    void ScriptInterface::SetForceMagnitude(Entity e, float mag)
    {
        float& magnitude = m_coord.ModifyComponent<Force>(e).magnitude;
        magnitude = mag;
    }

    // Lifetime ----
    float ScriptInterface::GetForceLifetime(Entity e)
    {
        float lifetime = m_coord.GetComponent<Force>(e).lifetime;
        return lifetime;
    }
    void ScriptInterface::SetForceLifetime(Entity e, float life)
    {
        float& lifetime = m_coord.ModifyComponent<Force>(e).lifetime;
        lifetime = life;
    }

    // Force ----
    bool ScriptInterface::GetForceActive(Entity e)
    {
        bool isActive = m_coord.GetComponent<Force>(e).isActive;
        return isActive;
    }
    void ScriptInterface::SetForceActive(Entity e, bool active)
    {
        bool& isActive = m_coord.ModifyComponent<Force>(e).isActive;
        isActive = active;
    }
#pragma endregion

    // --------------------------------------------------------------
    // Key Inputs
#pragma region Key/Buttons
    bool ScriptInterface::IsKeyPressed(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyPressed(key);
    }

    bool ScriptInterface::IsKeyDown(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyDown(key);
    }

    bool ScriptInterface::IsKeyReleased(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyReleased(key);
    }
#pragma endregion

    // --------------------------------------------------------------
    // Audio
    void ScriptInterface::PlaySound(const std::string& path) {
        //m_coord.GetSystem<AudioSystem>()->PlayAudio(soundPath);
        std::cout << "Playing sound: " << path << '\n';
    }

    // Time
    float ScriptInterface::GetDeltaTime() {
        return FixedDT;
    }

    // Logging
    void ScriptInterface::Log(const std::string& msg) {
        std::cout << "[Script] " << msg << '\n';
    }

    //Create Entity
    Entity ScriptInterface::CreateEntityArchetype(std::string archeTypeName, vec3 pos)
    {
        Entity entity= m_coord.GetSystem<EntityFactory>()->Create(archeTypeName);
        m_coord.ModifyComponent<Transform>(entity).pos = Vec3{ pos.x, pos.x, pos.z };
        return entity;
    }
    // --------------------------------------------------------------

}   // NightSnack::Scripting
