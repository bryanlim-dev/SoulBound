#pragma once

#include "ImportExport.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

#include <string>
#include <vector>
#include <unordered_map>

//typedef struct _MonoDomain MonoDomain;
//typedef struct _MonoAssembly MonoAssembly;
//typedef struct _MonoImage MonoImage;
//typedef struct _MonoClass MonoClass;
//typedef struct _MonoObject MonoObject;
//typedef struct _MonoMethod MonoMethod;

namespace NightSnack::Scripting {

    class CORE_API MonoScriptHost {
    public:
        MonoScriptHost();
        ~MonoScriptHost();

        // Disable copy/move
        MonoScriptHost(const MonoScriptHost&) = delete;
        MonoScriptHost& operator=(const MonoScriptHost&) = delete;

        // Initialize Mono runtime
        bool Initialize(const std::string& monoLibPath = "mono-lib");
        void Shutdown();

        // Load script assembly
        bool LoadAssembly(const std::string& assemblyPath);

        // Script instance management
        void* CreateScriptInstance(const std::string& namespaceName, const std::string& className);
        void DestroyScriptInstance(void* instance);

        // Call script methods
        void CallMethod(void* instance, const std::string& methodName);
        void CallMethodWithInt(void* instance, const std::string& methodName, int value);

        // Set script fields
        void SetScriptEntityId(void* instance, uint32_t entityId);

        // Update all active scripts
        void UpdateAllScripts();

        // Check if initialized
        bool IsInitialized() const { return m_initialized; }

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

        // Helper methods
        MonoClass* GetClass(const std::string& namespaceName, const std::string& className);
        MonoMethod* GetMethod(MonoClass* klass, const std::string& methodName, int paramCount = 0);
    };


}   // NightSnack::Scripting