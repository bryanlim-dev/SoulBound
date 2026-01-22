/******************************************************************************
 * File:        MonoScriptHost.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file declares the MonoScriptHost class which manages operations dealing
 * with script instances, Mono Assemblies, as well as handles for C# classes.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "ImportExport.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "../Core/ECS/Entity.h"
#include "../Core/ECS/Component.h"

// Forward declaration of types to avoid including Mono headers in public API
typedef struct _MonoDomain MonoDomain;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoImage MonoImage;
typedef struct _MonoClass MonoClass;
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;

namespace NightSnack::Scripting {

    class MonoScriptHost {
    public:
        // Constructor Destructor
        MonoScriptHost();
        ~MonoScriptHost();

        // Disable copy/move
        MonoScriptHost(MonoScriptHost const&) = delete;
        MonoScriptHost& operator=(const MonoScriptHost&) = delete;

        // Initialize Mono runtime
        bool Initialize();
        void Shutdown();

        // Load script assembly
        bool LoadAssembly(std::string const& path);

        // Script instance management
        void* CreateScriptInstance(std::string const& namespaceName, std::string const& className, Entity owner);
        void DestroyScriptInstance(void* instance);

        // Call script methods
        void CallMethod(void* instance, std::string const& methodName);
        // Set script fields
        void SetScriptEntityId(void* instance, uint32_t objID);

        // Update all active scripts
        void UpdateAllScripts();

        // Check if initialized
        bool IsInitialized() const { return m_initialized; }

        std::string GetScriptName(void* instance);

        void SetScriptValue(void* instance, ScriptModifiedValue val);

    private:
        MonoDomain* m_rootDomain;
        MonoDomain* m_appDomain;
        MonoAssembly* m_assembly;
        MonoImage* m_assemblyImage;

        // Cache of script classes
        std::unordered_map<std::string, MonoClass*> m_classCache;

        // Active script instances
        std::vector<MonoObject*> m_scriptInstances;

        bool m_initialized;

        // Helper functions
        MonoClass* GetClass(std::string const& namespaceName, std::string const& className);
        MonoMethod* GetMethod(MonoClass* c, std::string const& methodName, int paramCount = 0);


    };


}   // NightSnack::Scripting