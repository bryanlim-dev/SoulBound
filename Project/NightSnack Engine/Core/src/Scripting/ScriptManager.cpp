#pragma once

#include "Scripting/ScriptManager.h"

#include "Scripting/MonoScriptHost.h"
#include "Scripting/ScriptInterface.h"
#include "Scripting/MonoInternalCalls.h"

#include <iostream>

namespace NightSnack::Scripting {

    void ScriptManager::Update(int steps) {
        if (!m_attached) {
            // Attach scripts;
            for (Entity e : m_entities) {
                std::cout << "Script[" << m_coord.GetComponent<Script>(e).name << "] attached to Entity: " << e << '\n';
                AttachScript(e, m_coord.GetComponent<Script>(e).name);
            }
            m_attached = true;
        }

        // Skip updating of scripts if in debug mode;
        if (debugMode && !stepRequest) { return; }

        for (int s{}; s < steps; ++s) {
            // Update all active scripts
            m_monoHost->UpdateAllScripts();
        }

        if (debugMode) stepRequest = false; // reset step request
    }

	void ScriptManager::Initialize() {

        // Create script interface (bridge to engine)
        m_scriptInterface = std::make_unique<ScriptInterface>(m_coord);

        // Initialize Mono
        m_monoHost = std::make_unique<MonoScriptHost>();
        if (!m_monoHost->Initialize()) {
            std::cerr << "[ScriptManager] Failed to initialize Mono\n";
            return;
        }

        // Register internal calls (C# → C++ bridge)
        RegisterMonoInternalCalls(m_scriptInterface.get());

        // Load script assembly
        if (!m_monoHost->LoadAssembly("../bin/ManagedScripts.dll")) {
            std::cerr << "[ScriptManager] Failed to load ManagedScripts.dll\n";
            return;
        }
    }
    
    void ScriptManager::Shutdown() {

        // Clear all scripts
        m_entityScripts.clear();

        // Shutdown Mono
        m_monoHost.reset();
        m_scriptInterface.reset();
    }

    void* ScriptManager::AttachScript(Entity e, std::string const& scriptName) {

        // Create script instance -> from ManagedScripts namespace
        void* instance = m_monoHost->CreateScriptInstance("ManagedScripts", scriptName);
        if (!instance) {
            std::cerr << "[ScriptManager] Failed to create script instance\n";
            return nullptr;
        }

        // Set the entity ID on the script
        m_monoHost->SetScriptEntityId(instance, e);

        // Store mapping
        m_entityScripts[e] = instance;

        std::cout << "[ScriptManager] Script attached successfully\n";
        return instance;
    }

    void ScriptManager::RemoveScript(Entity e) {
        auto it = m_entityScripts.find(e);
        if (it == m_entityScripts.end()) {
            return;
        }

        // Destroy script instance
        m_monoHost->DestroyScriptInstance(it->second);
        // Remove from map
        m_entityScripts.erase(it);
    }

}	// NightSnack