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
