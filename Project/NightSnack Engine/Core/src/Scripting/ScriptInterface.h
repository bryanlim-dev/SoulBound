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

        // Input
        bool IsKeyPressed(int key);
        bool IsKeyDown(int key);
        bool IsKeyReleased(int key);

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
