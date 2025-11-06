#pragma once

#include "IScriptInterface.h"
#include "./Systems/AudioSystem.h"
#include "./Systems/Application/Application.h"
#include <iostream>

namespace NightSnack::Scripting {

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

    // Audio
    void ScriptInterface::PlaySound(const std::string& soundPath) {
        //m_coord.GetSystem<AudioSystem>()->PlayAudio(soundPath);
        std::cout << "Playing sound: " << soundPath << std::endl;
    }

    // Time
    float ScriptInterface::GetDeltaTime() {
        return m_coord.GetSystem<ApplicationSystem>()->GetDT();
    }

    // Logging
    void ScriptInterface::Log(const std::string& message) {
        std::cout << "[Script] " << message << std::endl;
    }

}   // NightSnack::Scripting
