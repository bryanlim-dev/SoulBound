/******************************************************************************
 * File:        MonoScriptHost.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 70%) - Added and implemeted base class
 *              Clement Ang Yang Ying (Secondary, 30%) - Modified and added value changing
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
        /************************************************************************/
        /*!
        \brief
        Constrcutor of MonoScriptHost, calls Initalize()
        */
        /************************************************************************/
        MonoScriptHost();
        /************************************************************************/
        /*!
        \brief
        Destructor of MonoScriptHost, calls Shutdown()
        */
        /************************************************************************/
        ~MonoScriptHost();

        // Disable copy/move
        MonoScriptHost(MonoScriptHost const&) = delete;
        MonoScriptHost& operator=(const MonoScriptHost&) = delete;

        /************************************************************************/
        /*!
        \brief
        Initialize Mono runtime
        */
        /************************************************************************/
        bool Initialize();

        /************************************************************************/
        /*!
        \brief
        Safely closes MonoScriptHost
        */
        /************************************************************************/
        void Shutdown();

        /************************************************************************/
        /*!
        \brief
        Load script assembly

        \param path
        File path of the dll

        \return
        True if successfuly loaded
        */
        /************************************************************************/
        bool LoadAssembly(std::string const& path);

        // Script instance management
        /************************************************************************/
        /*!
        \brief
        Creates a MonoObject instance of a script that returns a void*
        Does NOT modify the Script component of the Entity

        \param namespaceName
        Namespace of the script to make

        \paramclassName
        The class name of the script to make

        \param owner
        Entity ID of the owner of the script

        \return
        void* to the MonoObject instance of the script
        */
        /************************************************************************/
        void* CreateScriptInstance(std::string const& namespaceName, std::string const& className, Entity owner);

        /************************************************************************/
        /*!
        \brief
        Calls OnDestory on a script instance

        \param instance
        void* of MonoObject instance of script to call OnDestory
        */
        /************************************************************************/
        void DestroyScriptInstance(void* instance);

        /************************************************************************/
        /*!
        \brief
        Call script methods/functions


        \param instance
        void* of MonoObject instance of script to call

        \param methodName
        Name of method/funciton to call
        */
        /************************************************************************/
        void CallMethod(void* instance, std::string const& methodName);

        /************************************************************************/
        /*!
        \brief
        Set script Entity ID

        \param instance
        void* of MonoObject instance of script to set ID

        \param objID
        ID of enitty to set as
        */
        /************************************************************************/
        void SetScriptEntityId(void* instance, uint32_t objID);

        /************************************************************************/
        /*!
        \brief
        Loads function call on a script instance

        \param instance
        void* of MonoObject instance of script to call OnLoad
        */
        /************************************************************************/
        void LoadScript(void* instance);

        /************************************************************************/
        /*!
        \brief
        Calls onStart of a script instance

        \param instance
        void* of MonoObject instance of script to call OnStart
        */
        /************************************************************************/
        void StartScript(void* instance);
        
        /************************************************************************/
        /*!
        \brief
        Updates a script instance

        \param instance
        void* of MonoObject instance of script to call OnUpdate
        */
        /************************************************************************/
        void UpdateScript(void* instance);

        /************************************************************************/
        /*!
        \brief
        Check if MonoScriptHost initialized

        \return
        returns true if initalized
        */
        /************************************************************************/
        bool IsInitialized() const { return m_initialized; }

        /************************************************************************/
        /*!
        \brief
        Gets the name of a script insatance

        \param instance
        void* of MonoObject instance of script to modify
        */
        /************************************************************************/
        std::string GetScriptName(void* instance);

        /************************************************************************/
        /*!
        \brief
        Sets the value within a script instance

        \param instance
        void* of MonoObject instance of script to modify

        \param val
        struct containing data to modify
        */
        /************************************************************************/
        void SetScriptValue(void* instance, ScriptModifiedValue val);

    private:
        MonoDomain* m_rootDomain;
        MonoDomain* m_appDomain;
        MonoAssembly* m_assembly;
        MonoImage* m_assemblyImage;

        // Cache of script classes
        std::unordered_map<std::string, MonoClass*> m_classCache;


        bool m_initialized;

        // Helper functions
        MonoClass* GetClass(std::string const& namespaceName, std::string const& className);
        MonoMethod* GetMethod(MonoClass* c, std::string const& methodName, int paramCount = 0);


    };


}   // NightSnack::Scripting