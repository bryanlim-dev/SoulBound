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

        // Transform operations
        Trans GetTransform(GameObjectHandle e);
        void SetTransform(GameObjectHandle e, Trans& transform);

        //// Input
        //bool IsKeyPressed(int keyCode);
        //bool IsKeyDown(int keyCode);
        //bool IsKeyReleased(int keyCode);

        // Audio
        void PlaySound(const std::string& soundPath);

        // Time
        float GetDeltaTime();

        // Logging
        void Log(const std::string& message);

    private:
        Coordinator& m_coord;
    };

} // namespace NightSnack::Scripting
