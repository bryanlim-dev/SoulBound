/******************************************************************************
 * File:        MonoInternalCalls.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file defines internal calls that will link:
 * C# functions defined in Engine API in ManagedScripts/Engine.cs 
 * to NightSnack's native ScriptInterface's C++ functions. 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Scripting/MonoInternalCalls.h"

#include "Scripting/ScriptInterface.h"
#include "Scripting/ScriptTypes.h"

#include "mono/metadata/object.h"
#include "mono/metadata/appdomain.h"

#include <iostream>

// Global pointer to script interface (set during initialization)
static NightSnack::Scripting::ScriptInterface* g_scriptInterface = nullptr;

// Internal call functions from C# must use C linkage
extern "C" {
    // --------------------------------------------------------------

#pragma region Transform
	// Position --------------
	static void NightSnack_GetPosition(uint32_t id, NightSnack::Scripting::vec3* outpos) {
		if (!g_scriptInterface || !outpos) { return; }
		*outpos = g_scriptInterface->GetPosition(id);
	}
	static void NightSnack_SetPosition(uint32_t id, NightSnack::Scripting::vec3* pos) {
		if (!g_scriptInterface || !pos) { return; }
		g_scriptInterface->SetPosition(id, *pos);
	}

    // Rotation --------------
    static void NightSnack_GetRotation(uint32_t id, float* outrot) {
        if (!g_scriptInterface || !outrot) { return; }
        *outrot = g_scriptInterface->GetRotation(id);
    }
    static void NightSnack_SetRotation(uint32_t id, float* rot) {
        if (!g_scriptInterface || !rot) { return; }
        g_scriptInterface->SetRotation(id, *rot);
    }

    // Scale -----------------
    static void NightSnack_GetScale(uint32_t id, NightSnack::Scripting::vec2* scale) {
        if (!g_scriptInterface || !scale) { return; }
        *scale = g_scriptInterface->GetScale(id);
    }
    static void NightSnack_SetScale(uint32_t id, NightSnack::Scripting::vec2* scale) {
        if (!g_scriptInterface || !scale) { return; }
        g_scriptInterface->SetScale(id, *scale);
    }

    // Layer -----------------
    static void NightSnack_GetLayer(uint32_t id, uint8_t* layer) {
        if (!g_scriptInterface || !layer) { return; }
        *layer = static_cast<uint8_t>(g_scriptInterface->GetLayer(id));
    }
    static void NightSnack_SetLayer(uint32_t id, uint8_t* layer) {
        if (!g_scriptInterface || !layer) { return; }
        g_scriptInterface->SetLayer(id, static_cast<NightSnack::LayerType>(*layer));
    }
#pragma endregion //Transform

    // --------------------------------------------------------------

#pragma region Force
    // Direction --------------
    static void NightSnack_GetForceDirection(uint32_t id, NightSnack::Scripting::vec2* outdir) {
        if (!g_scriptInterface || !outdir) { return; }
        *outdir = g_scriptInterface->GetForceDirection(id);
    }
    static void NightSnack_SetForceDirection(uint32_t id, NightSnack::Scripting::vec2* dir) {
        if (!g_scriptInterface || !dir) { return; }
        g_scriptInterface->SetForceDirection(id, *dir);
    }

    // Magnitude --------------
    static void NightSnack_GetForceMagnitude(uint32_t id, float* outmag) {
        if (!g_scriptInterface || !outmag) { return; }
        *outmag = g_scriptInterface->GetForceMagnitude(id);
    }
    static void NightSnack_SetForceMagnitude(uint32_t id, float* mag) {
        if (!g_scriptInterface || !mag) { return; }
        g_scriptInterface->SetForceMagnitude(id, *mag);
    }

    // Lifetime --------------
    static void NightSnack_GetForceLifetime(uint32_t id, float* outLife) {
        if (!g_scriptInterface || !outLife) { return; }
        *outLife = g_scriptInterface->GetForceLifetime(id);
    }
    static void NightSnack_SetForceLifetime(uint32_t id, float* life) {
        if (!g_scriptInterface || !life) { return; }
        g_scriptInterface->SetForceLifetime(id, *life);
    }

    // Active --------------
    static void NightSnack_GetForceActive(uint32_t id, bool* outActive) {
        if (!g_scriptInterface || !outActive) { return; }
        *outActive = g_scriptInterface->GetForceActive(id);
    }
    static void NightSnack_SetForceActive(uint32_t id, bool* active) {
        if (!g_scriptInterface || !active) { return; }
        g_scriptInterface->SetForceActive(id, *active);
    }


#pragma endregion //Force

    // --------------------------------------------------------------

#pragma region Key/Buttons
    // Input - Is Key Pressed (just pressed this frame)
    static bool NightSnack_IsKeyPressed(int keyCode) {
        if (!g_scriptInterface) { return false; }
        return g_scriptInterface->IsKeyPressed(keyCode);
    }

    // Input - Is Key Down (held down)
    static bool NightSnack_IsKeyDown(int keyCode) {
        if (!g_scriptInterface) { return false; }
        return g_scriptInterface->IsKeyDown(keyCode);
    }

    // Input - Is Key Released
    static bool NightSnack_IsKeyReleased(int keyCode) {
        if (!g_scriptInterface) { return false; }
        return g_scriptInterface->IsKeyReleased(keyCode);
    }
#pragma endregion //Key/Buttons

    // --------------------------------------------------------------


    // Printing
    static void NightSnack_Log(MonoString* msg) {
        if (!g_scriptInterface) { return; }

        char* str = mono_string_to_utf8(msg);
        g_scriptInterface->Log(std::string{ str });
        mono_free(str);
    }

    // Audio - Play Sound
    static void NightSnack_PlaySound(MonoString* soundPath) {
        if (!g_scriptInterface) { return; }

        char* cStr = mono_string_to_utf8(soundPath);
        g_scriptInterface->PlaySound(std::string(cStr));
        mono_free(cStr);
    }

    // Time - Get Delta Time
    static float NightSnack_GetDeltaTime() {
        if (!g_scriptInterface) { return 0.0f; }
        return g_scriptInterface->GetDeltaTime();
    }

    //Create Entity
    static uint32_t NightSnack_CreateEntityArchetype(MonoString* archeType, NightSnack::Scripting::vec3* pos) {
        if (!g_scriptInterface) { return 0; }
        char* cStr = mono_string_to_utf8(archeType);
        return g_scriptInterface->CreateEntityArchetype(std::string(cStr), *pos);
    }
    // --------------------------------------------------------------


}	// extern "C"

namespace NightSnack::Scripting {

    void RegisterMonoInternalCalls(ScriptInterface* scriptInterface) {
        std::cout << "[Mono] Registering internal calls..." << std::endl;

        // Store global reference
        g_scriptInterface = scriptInterface;

        // Register all internal calls
        // Format: "Namespace.ClassName::MethodName"
#pragma region Transform
        mono_add_internal_call("ManagedScripts.Engine::GetPosition_Internal", (void*)NightSnack_GetPosition);
        mono_add_internal_call("ManagedScripts.Engine::SetPosition_Internal", (void*)NightSnack_SetPosition);

        mono_add_internal_call("ManagedScripts.Engine::GetRotation_Internal", (void*)NightSnack_GetRotation);
        mono_add_internal_call("ManagedScripts.Engine::SetRotation_Internal", (void*)NightSnack_SetRotation);

        mono_add_internal_call("ManagedScripts.Engine::GetScale_Internal", (void*)NightSnack_GetScale);
        mono_add_internal_call("ManagedScripts.Engine::SetScale_Internal", (void*)NightSnack_SetScale);

        mono_add_internal_call("ManagedScripts.Engine::GetLayer_Internal", (void*)NightSnack_GetLayer);
        mono_add_internal_call("ManagedScripts.Engine::SetLayer_Internal", (void*)NightSnack_SetLayer);
#pragma endregion

#pragma region Force
        mono_add_internal_call("ManagedScripts.Engine::GetForceDirection_Internal", (void*)NightSnack_GetForceDirection);
        mono_add_internal_call("ManagedScripts.Engine::SetForceDirection_Internal", (void*)NightSnack_SetForceDirection);

        mono_add_internal_call("ManagedScripts.Engine::GetForceMagnitude_Internal", (void*)NightSnack_GetForceMagnitude);
        mono_add_internal_call("ManagedScripts.Engine::SetForceMagnitude_Internal", (void*)NightSnack_SetForceMagnitude);

        mono_add_internal_call("ManagedScripts.Engine::GetForceLifetime_Internal", (void*)NightSnack_GetForceLifetime);
        mono_add_internal_call("ManagedScripts.Engine::SetForceLifetime_Internal", (void*)NightSnack_SetForceLifetime);

        mono_add_internal_call("ManagedScripts.Engine::GetForceActive_Internal", (void*)NightSnack_GetForceActive);
        mono_add_internal_call("ManagedScripts.Engine::SetForceActive_Internal", (void*)NightSnack_SetForceActive);
#pragma endregion


#pragma region Key/Inputs
        mono_add_internal_call("ManagedScripts.Engine::IsKeyPressed_Internal", (void*)NightSnack_IsKeyPressed);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyDown_Internal", (void*)NightSnack_IsKeyDown);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyReleased_Internal", (void*)NightSnack_IsKeyReleased);
#pragma endregion

        mono_add_internal_call("ManagedScripts.Engine::Log_Internal", (void*)NightSnack_Log);
        mono_add_internal_call("ManagedScripts.Engine::PlaySound_Internal", (void*)NightSnack_PlaySound);
        mono_add_internal_call("ManagedScripts.Engine::GetDeltaTime_Internal", (void*)NightSnack_GetDeltaTime);

        mono_add_internal_call("ManagedScripts.Engine::CreateEntityArchetype_Internal", (void*)NightSnack_CreateEntityArchetype);


        std::cout << "[Mono] Internal calls registered successfully" << std::endl;
    }

} // NightSnack::Scripting
