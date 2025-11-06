#pragma once

#include "ScriptInterface.h"

#include "./Systems/AudioSystem.h"
#include "./Systems/Application/Application.h"
#include "./Systems/Application/Input.h"

#include <iostream>

namespace NightSnack::Scripting {

    // --------------------------------------------------------------
    // Transform operations
    Trans ScriptInterface::GetTransform(GameObjectHandle obj) {
        Entity e = static_cast<Entity>(obj.id);

        Transform t = m_coord.GetComponent<Transform>(e);
        return Trans::FromNative(t);
    }
    void ScriptInterface::SetTransform(GameObjectHandle obj, Trans& transform) {

        Entity e = static_cast<Entity>(obj.id);

        Transform t = transform.ToNative();
        m_coord.ModifyComponent<Transform>(e).pos = t.pos;
        m_coord.ModifyComponent<Transform>(e).rot = t.rot;
        m_coord.ModifyComponent<Transform>(e).scale = t.scale;
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
        return m_coord.GetSystem<ApplicationSystem>()->GetDT();
    }

    // Logging
    void ScriptInterface::Log(const std::string& msg) {
        std::cout << "[Script] " << msg << '\n';
    }
    // --------------------------------------------------------------

}   // NightSnack::Scripting
