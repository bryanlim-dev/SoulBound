/******************************************************************************
 * File:        ScriptManager.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 80%) - Created class and logic
 *              Clement Ang Yang Ying (Secondary, 20%) - Added MonoExposeBridge integration/maps for scripts and modified functions
 * 
 * Description:
 * This file defines the member functions of ScriptManager class.
 * Functions include:
 * - Update -> called by CoreEngine::Update()
 * - Initialize and Shutdown
 * - AttachScript and RemoveAllScripts
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Scripting/ScriptManager.h"

#include "Scripting/MonoScriptHost.h"
#include "Scripting/ScriptInterface.h"
#include "Scripting/MonoInternalCalls.h"

#include <iostream>

namespace NightSnack::Scripting {

    // Main update to be called by Core engine (Updates all scripts)
    void ScriptManager::Update() {
        // Reattach entity scripts when Level is reloaded
        if (!m_attached || entityNum != m_entities.size()) {
            for (Entity e{}; e < MAX_ENTITIES; ++e) {
                RemoveAllScripts(e);
            }

            entityNum = m_entities.size();
            // Attach scripts;
            for (Entity e : m_entities) {
                //std::cout << "Script[" << m_coord.GetComponent<Script>(e).name << "] attached to Entity: " << e << '\n';
                //AttachScript(e, m_coord.GetComponent<Script>(e).name);
                for (const std::string& scriptName : m_coord.GetComponent<Script>(e).names)
                {
                    AttachScript(e, scriptName);
                }
            }
            m_attached = true;
        }

        // Skip updating of scripts if in debug mode;
        if (debugMode && !stepRequest) { return; }

        for (int s{}; s < m_coord.steps; ++s) {
            // Update all active scripts
            m_monoHost->UpdateAllScripts();
        }

        if (debugMode) stepRequest = false; // reset step request
    }

    std::vector<void*> ScriptManager::GetEntityScripts(Entity e)
    {
        if (m_entityScripts.find(e) == m_entityScripts.end())
        {
            return std::vector<void*>();
        }

        return m_entityScripts[e];
    }

    std::string ScriptManager::GetScriptName(void* script)
    {
        if (!script)
         return "";
        if (m_scriptNameMap.find(script) == m_scriptNameMap.end())
         return "";

         return m_scriptNameMap[script];
    }

    MonoExposeBridge& ScriptManager::GetMonoExposeBridge()
    {
        return m_monoExposeBridge;
    }

    void ScriptManager::LoadEntityExposedFields(Entity e)
    {
        UnloadEntityExposedFields();

        //No scripts
        if (m_entityScripts.find(e) == m_entityScripts.end())
            return;

        //Loop thru each script in the vector
        for (void* instance : m_entityScripts[e])
        {
            m_monoExposeBridge.LoadScriptExposedFields(instance);
        }

        //auto range = m_entityScripts.equal_range(e);
        //for (auto it = range.first; it != range.second; ++it)
        //    m_monoExposeBridge.LoadScriptExposedFields(it->second);
    }

    void ScriptManager::UnloadEntityExposedFields()
    {
        m_monoExposeBridge.UnloadAll();
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
        if (!m_monoHost->LoadAssembly("./assets/Scripts/ManagedScripts.dll")) {
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

    bool ScriptManager::CreateScript(Entity e, std::string scriptName)
    {
        if (AttachScript(e, scriptName) != nullptr)
        {
            if (!m_coord.HasComponent<Script>(e))
            {
                m_coord.AddComponent(e, Script());
            }

            m_coord.ModifyComponent<Script>(e).names.push_back(scriptName);
            return true;
        }
        return false;
    }

    // Remove an entity's script
    void ScriptManager::RemoveAllScripts(Entity e) {
        auto it = m_entityScripts.find(e);
        if (it == m_entityScripts.end()) {
            return;
        }

        // Destroy script instance
        for (void* instance : it->second)
        {
            //std::cout << "ScriptManager Desotry:" << m_scriptNameMap[instance] << std::endl;
            m_monoHost->DestroyScriptInstance(instance);
            m_scriptNameMap.erase(instance);
        }
        // Remove from map
        m_entityScripts[e].clear();

        //m_coord.ModifyComponent<Script>(e).names.clear();
    }

    void ScriptManager::RemoveScript(Entity e, std::string scriptName)
    {
        //Check if it exists
        auto it = m_entityScripts.find(e);
        if (it == m_entityScripts.end()) {
            return;
        }

        //Loop thru scripts to find same name
        auto& scripts = it->second; //Vector of scripts for this enttity
        for (auto scriptIt = scripts.begin(); scriptIt != scripts.end(); ++scriptIt)
        {
            void* instance = *scriptIt;
            std::string name = m_monoHost->GetScriptName(instance);
            if (name == scriptName)
            {
                //Destroy script instance & remove from container
                m_monoHost->DestroyScriptInstance(instance);
                m_scriptNameMap.erase(instance);
                scripts.erase(scriptIt);

                //Remove from script component names list (This prevents reload from reloading the thing)
                //auto& names = m_coord.ModifyComponent<Script>(e).names;
                //names.remove(scriptName);
                break;
            }
		}
    }

    // Attach a script to an entity (ScriptName is the name of the C# class, but just use Script.name component)
    // i.e. m_coord.GetComponent<Script>().name;
    void* ScriptManager::AttachScript(Entity e, std::string const& scriptName) {

        // Create script instance -> from ManagedScripts namespace
        void* instance = m_monoHost->CreateScriptInstance("ManagedScripts", scriptName, e);
        if (!instance) {
            std::cerr << "[ScriptManager] Failed to create script instance\n";
            return nullptr;
        }

        //Called in CreateScriptInstance
        // Set the entity ID on the script
        //m_monoHost->SetScriptEntityId(instance, e);

        //If dont have a entry in the map, add it
        if (m_entityScripts.find(e) == m_entityScripts.end())
        {
            m_entityScripts.insert({ e, std::vector<void*>() });
        }

        // Store mapping
        m_entityScripts[e].push_back(instance);
        m_scriptNameMap.insert({ instance,  m_monoHost->GetScriptName(instance) }); //Insert name into a map

        //m_coord.ModifyComponent<Script>(e).names.push_back(scriptName);

        //std::cout << "ScriptManager Create:" << m_scriptNameMap[instance] << std::endl;

        ////std::cout << "[ScriptManager] Script attached successfully\n";
        return instance;
    }

}	// NightSnack