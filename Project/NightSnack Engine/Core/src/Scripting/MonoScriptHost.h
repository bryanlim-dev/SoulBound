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
        MonoScriptHost(MonoScriptHost const&) = delete;
        MonoScriptHost& operator=(MonoScriptHost const&) = delete;

        bool InitMono();
        void Shutdown();

        bool LoadAssembly(std::string const& asspath);

        void* CreateScriptInstance(std::string const&);

    private:
        MonoDomain* m_rootDomain;
        MonoDomain* m_appDomain;
        MonoAssembly* m_assembly;
        MonoImage* m_image;

        bool m_initialized;

    };


}   // NightSnack::Scripting