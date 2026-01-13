/******************************************************************************
 * File:        MonoScriptHost.cpp
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file defines the member functions of MonoScriptHost.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Scripting/MonoScriptHost.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/mono-config.h"

#include <iostream>
#include <algorithm>
#include <Core/ECS/Entity.h>

namespace NightSnack::Scripting {

    // Sets default values for Mono data members and calls Initialize() member funciton.
    MonoScriptHost::MonoScriptHost()
        : m_rootDomain(nullptr),
        m_appDomain(nullptr),
        m_assembly(nullptr),
        m_assemblyImage(nullptr),
        m_initialized(false) {
        Initialize();
    }
    // Calls Shut down to clean up.
    MonoScriptHost::~MonoScriptHost() {
        Shutdown();
    }

    // Initializes Mono and creates app domain.
    bool MonoScriptHost::Initialize() {
        if (m_initialized) {
            //std::cout << "[Mono] Already initialized\n";
            return true;
        }

        // Set Mono assembly search path
        mono_set_assemblies_path("./lib");

        // Initialize Mono JIT
        m_rootDomain = mono_jit_init("NightSnackEngine");
        if (!m_rootDomain) {
            std::cerr << "[Mono] ERROR: Failed to initialize Mono JIT\n";
            return false;
        }

        // Create application domain (allows unloading assemblies)
        m_appDomain = mono_domain_create_appdomain((char*)"ScriptDomain", nullptr);
        if (!m_appDomain) {
            std::cerr << "[Mono] ERROR: Failed to create app domain\n";
            return false;
        }

        // Set as active domain
        if (!mono_domain_set(m_appDomain, false)) {
            std::cerr << "[Mono] ERROR: Failed to set app domain\n";
            return false;
        }

        m_initialized = true;
        return true;
    }

    // Clears all Script instances and Mono class cache.
    void MonoScriptHost::Shutdown() {
        if (!m_initialized) return;

        // Clear script instances
        m_scriptInstances.clear();
        m_classCache.clear();

        // Unload app domain
        if (m_appDomain) {
            mono_domain_set(mono_get_root_domain(), false);
            mono_domain_unload(m_appDomain);
            m_appDomain = nullptr;
        }

        // Cleanup JIT
        if (m_rootDomain) {
            mono_jit_cleanup(m_rootDomain);
            m_rootDomain = nullptr;
        }

        m_assembly = nullptr;
        m_assemblyImage = nullptr;
        m_initialized = false;
    }

    // Loads an assembly given a specified filepath;
    bool MonoScriptHost::LoadAssembly(std::string const& path) {
        if (!m_initialized) {
            std::cerr << "[Mono] ERROR: Cannot load assembly - not initialized\n";
            return false;
        }

        // Open assembly
        m_assembly = mono_domain_assembly_open(m_appDomain, path.c_str());
        if (!m_assembly) {
            std::cerr << "[Mono] ERROR: Failed to load assembly: " << path << '\n';
            return false;
        }
        // Get assembly image
        m_assemblyImage = mono_assembly_get_image(m_assembly);
        if (!m_assemblyImage) {
            std::cerr << "[Mono] ERROR: Failed to get assembly image\n";
            return false;
        }

        return true;
    }

    // Creates an instance of a script. namespaceName -> ManagedScripts, className -> name of C# script class
    void* MonoScriptHost::CreateScriptInstance(std::string const& namespaceName, std::string const& className, Entity owner) {
        if (!m_initialized || !m_assembly) {
            std::cerr << "[Mono] ERROR: Cannot create instance - not initialized or no assembly loaded\n";
            return nullptr;
        }

        // Get class
        MonoClass* c = GetClass(namespaceName, className);
        if (!c) {
            std::cerr << "[Mono] ERROR: Failed to find class: " << namespaceName << "." << className << '\n';
            return nullptr;
        }

        // Allocate object
        MonoObject* instance = mono_object_new(m_appDomain, c);
        if (!instance) {
            std::cerr << "[Mono] ERROR: Failed to create instance\n";
            return nullptr;
        }

        // Call constructor (if exists)
        mono_runtime_object_init(instance);

        // Store instance
        m_scriptInstances.push_back(instance);

        //Set the entity ID on the script

        SetScriptEntityId(instance, owner);
//m_    
        // Call OnLoad
        CallMethod(instance, "OnLoad");

        return instance;
    }

    void MonoScriptHost::DestroyScriptInstance(void* instance) {
        if (!instance) return;

        MonoObject* obj = static_cast<MonoObject*>(instance);

        // Call OnDestroy
        CallMethod(instance, "OnDestroy");

        // Remove from active instances
        auto it = std::find(m_scriptInstances.begin(), m_scriptInstances.end(), obj);
        if (it != m_scriptInstances.end()) {
            m_scriptInstances.erase(it);
        }
    }

    // Calls member functions of C# script classes. Name must match.
    // e.g -> CallMethod(instance, "OnUpdate");
    void MonoScriptHost::CallMethod(void* instance, std::string const& methodName) {
        if (!instance) return;

        MonoObject* obj = static_cast<MonoObject*>(instance);
        MonoClass* c = mono_object_get_class(obj);

        MonoMethod* method = GetMethod(c, methodName, 0);
        if (!method) {
            return;
        }

        // Invoke method with no parameters
        MonoObject* exception = nullptr;
        mono_runtime_invoke(method, obj, nullptr, &exception);

        // Logs error in Cerr in case of exceptions
        if (exception) {
            MonoClass* exceptionClass = mono_object_get_class(exception);
            const char* exceptionName = mono_class_get_name(exceptionClass);

            // Try to get exception message
            MonoProperty* messageProp = mono_class_get_property_from_name(exceptionClass, "Message");
            if (messageProp) {
                MonoMethod* getter = mono_property_get_get_method(messageProp);
                MonoString* messageStr = (MonoString*)mono_runtime_invoke(getter, exception, nullptr, nullptr);
                char* message = mono_string_to_utf8(messageStr);
                std::cerr << "[Mono] Script exception in " << methodName << ": " << exceptionName << " - " << message << '\n';
                mono_free(message);
            }
            else {
                std::cerr << "[Mono] Script exception in " << methodName << ": " << exceptionName << '\n';
            }
        }
    }

    // Links entityID field within the Script class in C#
    void MonoScriptHost::SetScriptEntityId(void* instance, uint32_t objID) {
        if (!instance) return;

        MonoObject* obj = static_cast<MonoObject*>(instance);
        MonoClass* c = mono_object_get_class(obj);

        // Find EntityId field
        MonoClassField* field = mono_class_get_field_from_name(c, "EntityId");
        if (!field) {
            std::cerr << "[Mono] WARNING: EntityId field not found in script class\n";
            return;
        }

        // Set field value
        mono_field_set_value(obj, field, &objID);
    }

    void MonoScriptHost::UpdateAllScripts() {
        // Call OnUpdate on all active scripts
        for (MonoObject* instance : m_scriptInstances) {
            CallMethod(instance, "OnUpdate");
        }
    }

    std::string MonoScriptHost::GetScriptName(void* instance)
    {
        if (instance)
        {
            MonoObject* obj = static_cast<MonoObject*>(instance);
            MonoClass* c = mono_object_get_class(obj);
            return mono_class_get_name(c);
        }
        else
            return "";
    }

    // Helper functions ----------------------------------

    MonoClass* MonoScriptHost::GetClass(std::string const& namespaceName, std::string const& className) {
        std::string fullName = namespaceName + "." + className;

        // Check cache
        auto it = m_classCache.find(fullName);
        if (it != m_classCache.end()) {
            return it->second;
        }

        // Find class
        MonoClass* c = mono_class_from_name(
            m_assemblyImage,
            namespaceName.c_str(),
            className.c_str()
        );

        if (c) {
            m_classCache[fullName] = c;
            //std::cout << "[Mono] Cached class: " << fullName << '\n';
        }
        else {
            std::cerr << "[Mono] ERROR: Class not found: " << fullName << '\n';
        }

        return c;
    }

    MonoMethod* MonoScriptHost::GetMethod(MonoClass* c, std::string const& methodName, int paramCount) {
        if (!c) return nullptr;

        MonoMethod* method = mono_class_get_method_from_name(c, methodName.c_str(), paramCount);
        return method;
    }

}   // NightSnack::Scripting
