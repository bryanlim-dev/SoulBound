#include "Scripting/MonoInternalCalls.h"
#include "Scripting/ScriptInterface.h"
#include "Scripting/ScriptTypes.h"

#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <iostream>

// Global pointer to script interface (set during initialization)
static NightSnack::Scripting::ScriptInterface* g_scriptInterface = nullptr;

// Internal call functions from C# must use C linkage
extern "C" {

	// Transform - Get
	static void NightSnack_GetPosition(uint32_t id, NightSnack::Scripting::vec3* outpos) {
		if (!g_scriptInterface || !outpos) { return; }

		NightSnack::Scripting::Trans transform = g_scriptInterface->GetTransform(id);
		*outpos = transform.pos;
	}

	// Transform - Set
	static void NightSnack_SetPosition(uint32_t id, NightSnack::Scripting::vec3* pos) {
		if (!g_scriptInterface || !pos) { return; }

		NightSnack::Scripting::Trans transform = g_scriptInterface->GetTransform(id);
		transform.pos = *pos;
		g_scriptInterface->SetTransform(id, transform);
	}
    // --------------------------------------------------------------

    // Input - Is Key Pressed (just pressed this frame)
    static bool NightSnack_IsKeyPressed(int keyCode) {
        if (!g_scriptInterface) return false;
        return g_scriptInterface->IsKeyPressed(keyCode);
    }

    // Input - Is Key Down (held down)
    static bool NightSnack_IsKeyDown(int keyCode) {
        if (!g_scriptInterface) return false;
        return g_scriptInterface->IsKeyDown(keyCode);
    }

    // Input - Is Key Released
    static bool NightSnack_IsKeyReleased(int keyCode) {
        if (!g_scriptInterface) return false;
        return g_scriptInterface->IsKeyReleased(keyCode);
    }
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
        if (!g_scriptInterface) return;

        char* cStr = mono_string_to_utf8(soundPath);
        g_scriptInterface->PlaySound(std::string(cStr));
        mono_free(cStr);
    }

    // Time - Get Delta Time
    static float NightSnack_GetDeltaTime() {
        if (!g_scriptInterface) return 0.0f;
        return g_scriptInterface->GetDeltaTime();
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
        mono_add_internal_call("ManagedScripts.Engine::GetPosition_Internal", (void*)NightSnack_GetPosition);
        mono_add_internal_call("ManagedScripts.Engine::SetPosition_Internal", (void*)NightSnack_SetPosition);

        mono_add_internal_call("ManagedScripts.Engine::IsKeyPressed_Internal", (void*)NightSnack_IsKeyPressed);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyDown_Internal", (void*)NightSnack_IsKeyDown);
        mono_add_internal_call("ManagedScripts.Engine::IsKeyReleased_Internal", (void*)NightSnack_IsKeyReleased);

        mono_add_internal_call("ManagedScripts.Engine::Log_Internal", (void*)NightSnack_Log);
        mono_add_internal_call("ManagedScripts.Engine::PlaySound_Internal", (void*)NightSnack_PlaySound);
        mono_add_internal_call("ManagedScripts.Engine::GetDeltaTime_Internal", (void*)NightSnack_GetDeltaTime);

        std::cout << "[Mono] Internal calls registered successfully" << std::endl;
    }

} // NightSnack::Scripting
