#pragma once

#include "Scripting/MonoScriptHost.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>

#include <iostream>
#include <algorithm>

namespace NightSnack::Scripting {

    MonoScriptHost::MonoScriptHost()
        : m_rootDomain(nullptr),
        m_appDomain(nullptr),
        m_assembly(nullptr),
        m_assemblyImage(nullptr),
        m_initialized(false) {}

    MonoScriptHost::~MonoScriptHost() {
        Shutdown();
    }

    bool MonoScriptHost::Initialize(const std::string& monoLibPath) {
        if (m_initialized) {
            std::cout << "[Mono] Already initialized" << std::endl;
            return true;
        }

        std::cout << "[Mono] Initializing..." << std::endl;

        // Set Mono assembly search path
        mono_set_assemblies_path(monoLibPath.c_str());

        // Initialize Mono JIT
        m_rootDomain = mono_jit_init("NightSnackEngine");
        if (!m_rootDomain) {
            std::cerr << "[Mono] ERROR: Failed to initialize Mono JIT" << std::endl;
            return false;
        }

        std::cout << "[Mono] JIT initialized" << std::endl;

        // Create application domain (allows unloading assemblies)
        m_appDomain = mono_domain_create_appdomain((char*)"ScriptDomain", nullptr);
        if (!m_appDomain) {
            std::cerr << "[Mono] ERROR: Failed to create app domain" << std::endl;
            return false;
        }

        // Set as active domain
        if (!mono_domain_set(m_appDomain, false)) {
            std::cerr << "[Mono] ERROR: Failed to set app domain" << std::endl;
            return false;
        }

        std::cout << "[Mono] App domain created and set" << std::endl;

        m_initialized = true;
        return true;
    }

    void MonoScriptHost::Shutdown() {
        if (!m_initialized) return;

        std::cout << "[Mono] Shutting down..." << std::endl;

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

        std::cout << "[Mono] Shutdown complete" << std::endl;
    }

    bool MonoScriptHost::LoadAssembly(const std::string& assemblyPath) {
        if (!m_initialized) {
            std::cerr << "[Mono] ERROR: Cannot load assembly - not initialized" << std::endl;
            return false;
        }

        std::cout << "[Mono] Loading assembly: " << assemblyPath << std::endl;

        // Open assembly
        m_assembly = mono_domain_assembly_open(m_appDomain, assemblyPath.c_str());
        if (!m_assembly) {
            std::cerr << "[Mono] ERROR: Failed to load assembly: " << assemblyPath << std::endl;
            return false;
        }

        // Get assembly image
        m_assemblyImage = mono_assembly_get_image(m_assembly);
        if (!m_assemblyImage) {
            std::cerr << "[Mono] ERROR: Failed to get assembly image" << std::endl;
            return false;
        }

        std::cout << "[Mono] Assembly loaded successfully" << std::endl;
        return true;
    }

    void* MonoScriptHost::CreateScriptInstance(const std::string& namespaceName, const std::string& className) {
        if (!m_initialized || !m_assembly) {
            std::cerr << "[Mono] ERROR: Cannot create instance - not initialized or no assembly loaded" << std::endl;
            return nullptr;
        }

        std::cout << "[Mono] Creating instance: " << namespaceName << "." << className << std::endl;

        // Get class
        MonoClass* klass = GetClass(namespaceName, className);
        if (!klass) {
            std::cerr << "[Mono] ERROR: Failed to find class: " << namespaceName << "." << className << std::endl;
            return nullptr;
        }

        // Allocate object
        MonoObject* instance = mono_object_new(m_appDomain, klass);
        if (!instance) {
            std::cerr << "[Mono] ERROR: Failed to create instance" << std::endl;
            return nullptr;
        }

        // Call constructor (if exists)
        mono_runtime_object_init(instance);

        // Store instance
        m_scriptInstances.push_back(instance);

        std::cout << "[Mono] Instance created successfully" << std::endl;

        // Call OnStart
        CallMethod(instance, "OnStart");

        return instance;
    }

    void MonoScriptHost::DestroyScriptInstance(void* instance) {
        if (!instance) return;

        MonoObject* obj = static_cast<MonoObject*>(instance);

        std::cout << "[Mono] Destroying script instance" << std::endl;

        // Call OnDestroy
        CallMethod(instance, "OnDestroy");

        // Remove from active instances
        auto it = std::find(m_scriptInstances.begin(), m_scriptInstances.end(), obj);
        if (it != m_scriptInstances.end()) {
            m_scriptInstances.erase(it);
        }

        // Note: Mono GC will handle actual memory cleanup
    }

    void MonoScriptHost::CallMethod(void* instance, const std::string& methodName) {
        if (!instance) return;

        MonoObject* obj = static_cast<MonoObject*>(instance);
        MonoClass* klass = mono_object_get_class(obj);

        MonoMethod* method = GetMethod(klass, methodName, 0);
        if (!method) {
            // Method doesn't exist - that's okay (e.g., OnStart not implemented)
            return;
        }

        // Invoke method with no parameters
        MonoObject* exception = nullptr;
        mono_runtime_invoke(method, obj, nullptr, &exception);

        if (exception) {
            MonoClass* exceptionClass = mono_object_get_class(exception);
            const char* exceptionName = mono_class_get_name(exceptionClass);

            // Try to get exception message
            MonoProperty* messageProp = mono_class_get_property_from_name(exceptionClass, "Message");
            if (messageProp) {
                MonoMethod* getter = mono_property_get_get_method(messageProp);
                MonoString* messageStr = (MonoString*)mono_runtime_invoke(getter, exception, nullptr, nullptr);
                char* message = mono_string_to_utf8(messageStr);
                std::cerr << "[Mono] Script exception in " << methodName << ": " << exceptionName << " - " << message << std::endl;
                mono_free(message);
            }
            else {
                std::cerr << "[Mono] Script exception in " << methodName << ": " << exceptionName << std::endl;
            }
        }
    }

    void MonoScriptHost::CallMethodWithInt(void* instance, const std::string& methodName, int value) {
        if (!instance) return;

        MonoObject* obj = static_cast<MonoObject*>(instance);
        MonoClass* klass = mono_object_get_class(obj);

        MonoMethod* method = GetMethod(klass, methodName, 1);
        if (!method) {
            return;
        }

        // Prepare parameters
        void* args[1];
        args[0] = &value;

        MonoObject* exception = nullptr;
        mono_runtime_invoke(method, obj, args, &exception);

        if (exception) {
            std::cerr << "[Mono] Exception calling " << methodName << std::endl;
        }
    }

    void MonoScriptHost::SetScriptEntityId(void* instance, uint32_t entityId) {
        if (!instance) return;

        MonoObject* obj = static_cast<MonoObject*>(instance);
        MonoClass* klass = mono_object_get_class(obj);

        // Find EntityId field
        MonoClassField* field = mono_class_get_field_from_name(klass, "EntityId");
        if (!field) {
            std::cerr << "[Mono] WARNING: EntityId field not found in script class" << std::endl;
            return;
        }

        // Set field value
        mono_field_set_value(obj, field, &entityId);
    }

    void MonoScriptHost::UpdateAllScripts() {
        // Call OnUpdate on all active scripts
        for (MonoObject* instance : m_scriptInstances) {
            CallMethod(instance, "OnUpdate");
        }
    }

    MonoClass* MonoScriptHost::GetClass(const std::string& namespaceName, const std::string& className) {
        std::string fullName = namespaceName + "." + className;

        // Check cache
        auto it = m_classCache.find(fullName);
        if (it != m_classCache.end()) {
            return it->second;
        }

        // Find class
        MonoClass* klass = mono_class_from_name(
            m_assemblyImage,
            namespaceName.c_str(),
            className.c_str()
        );

        if (klass) {
            m_classCache[fullName] = klass;
            std::cout << "[Mono] Cached class: " << fullName << std::endl;
        }
        else {
            std::cerr << "[Mono] ERROR: Class not found: " << fullName << std::endl;
        }

        return klass;
    }

    MonoMethod* MonoScriptHost::GetMethod(MonoClass* klass, const std::string& methodName, int paramCount) {
        if (!klass) return nullptr;

        MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), paramCount);
        return method;
    }

}   // NightSnack::Scripting
