#pragma once

#include "ScriptInterface.h"

#include "./Systems/AudioSystem.h"
#include "./Systems/Application/Application.h"
#include "./Systems/Application/Input.h"

#include <iostream>

namespace NightSnack::Scripting {

    // --------------------------------------------------------------
    // Transform operations
    Trans ScriptInterface::GetTransform(Entity e) {

        Transform t = m_coord.GetComponent<Transform>(e);
        Trans result;

        result.pos = vec3(t.pos.x, t.pos.y, t.pos.z);
        result.rot = t.rot;
        result.scale = vec2(t.scale.x, t.scale.y);

        return result;
    }
    void ScriptInterface::SetTransform(Entity e, Trans& t) {

        Transform& trans = m_coord.ModifyComponent<Transform>(e);

        trans.pos = Vec3(t.pos.x, t.pos.y, t.pos.z);
        trans.rot = t.rot;
        trans.scale = Vec2(t.scale.x, t.scale.y);
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

    // --------------------------------------------------------------
    // Key Inputs
    bool ScriptInterface::IsKeyPressed(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyPressed(key);
    }

    bool ScriptInterface::IsKeyDown(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyDown(key);
    }

    bool ScriptInterface::IsKeyReleased(int key) {
        return m_coord.GetSystem<InputSystem>()->IsKeyReleased(key);
    }
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
    // --------------------------------------------------------------

}   // NightSnack::Scripting
