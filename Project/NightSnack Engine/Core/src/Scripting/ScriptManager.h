/******************************************************************************
 * File:        ScriptManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 80%) - Created class and logic
 *              Clement Ang Yang Ying (Secondary, 20%) - Added MonoExposeBridge integration/maps for scripts and modified functions
 *
 * Description:
 * This file defines class ScriptManager. 
 * 
 * This class is the System that manages
 * Script operations and begins the chain from the CoreEngine that eventually
 * links to Scripts in ManagedScripts
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "ScriptInterface.h"
#include "Scripting/MonoScriptHost.h"
#include "Scripting/MonoExposeBridge.h"
#include "ImportExport.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace NightSnack::Scripting {

	class ScriptManager : public ISystem {

	public:
		/************************************************************************/
		/*!
		\brief
		Constructor for ScriptManager, requires coordinator, event dispatcher.
		Creates ScriptInterface, MonoHost and then registers internal calls in
		Initialize().
		*/
		/************************************************************************/
		ScriptManager(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp } {

			// Physics components of interest;
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Script>());

			//// Event dispatcher initialisations
			//m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
			//	if (event.pressed == InputEvent::InputActionState::Pressed) {
			//		switch (event.inputAction) {
			//		case InputEvent::InputAction::PhysicsDebugMode:
			//			debugMode = !debugMode;
			//			break;
			//		case InputEvent::InputAction::PhysicsDebugStep:
			//			stepRequest = true;
			//			break;
			//		case InputEvent::InputAction::EndStressTest:
			//			m_attached = false;
			//			break;
			//		}
			//	}
			//	});

			Initialize();
			entityNum = m_entities.size();

			AddLayer(LayerType::BACKGROUND);
			AddLayer(LayerType::GAMELAYER);
			AddLayer(LayerType::FOREGROUND);
			AddLayer(LayerType::GAMEUI);
		}
		// Destructor that calls shutdown member function;
		~ScriptManager() { 
			Shutdown();
			m_scriptInstances.clear();
			m_entityScripts.clear();
			m_scriptOwner.clear();
			m_scriptNameMap.clear();
			m_scriptsToStart.clear();
			m_scriptsToLoad.clear();
		}
		
		/************************************************************************/
		/*!
		\brief
		Main Update function for script manager
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Creates a new script and modifies the Script component for the entity

		\param e
		Entity id of owner of script

		\param scriptName
		Class name of script to make

		\return
		true if able to create script
		false if not
		*/
		/************************************************************************/
		bool CreateScript(Entity e, std::string scriptName);

		/************************************************************************/
		/*!
		\brief
		Remove all scripts instances (void*) from entity
		Does NOT effect the Scripts component

		\param e
		Entity to remove scripts from
		*/
		/************************************************************************/
		void RemoveAllScripts(Entity e);

		/************************************************************************/
		/*!
		\brief
		Remove the first matching script from entity
		Does NOT effect the Scripts component

		\param e
		Entity to remove scripts from

		\param scriptName
		Class name of script to remove
		*/
		/************************************************************************/
		void RemoveScript(Entity e, std::string scriptName);

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const { return "ScriptManager"; }


		/************************************************************************/
		/*!
		\brief
		Gets the all scripts of an entity

		\param e
		Entity ID to find scripts

		\return
		std::vector of void* of MonoObject script instances belonging to the entity
		*/
		/************************************************************************/
		std::vector<void*> GetEntityScripts(Entity e);

		/************************************************************************/
		/*!
		\brief
		Gets the class name of a script

		\param script
		void* of MonoObject script instance to get name from

		\return
		std::string of the class name of script instance
		*/
		/************************************************************************/
		std::string GetScriptName(void* script);

		/************************************************************************/
		/*!
		\brief
		Gets the Entity ID of the owner of a script

		\param script
		void* of MonoObject script instance to get entity owner from

		\return
		Entity ID of the owner of a script
		*/
		/************************************************************************/
		Entity GetScriptOwner(void* script);


		/************************************************************************/
		/*!
		\brief
		Returns a refrence to MonoExposeBridge

		\return
		Returns a refrence to MonoExposeBridge
		*/
		/************************************************************************/
		MonoExposeBridge& GetMonoExposeBridge();


		/************************************************************************/
		/*!
		\brief
		Calls MonoExpooseBridge to cache exposed fields in scripts from an entity
		*/
		/************************************************************************/
		void LoadEntityExposedFields(Entity e);

		/************************************************************************/
		/*!
		\brief
		Calls MonoExpooseBridge to unload all cached exposed fields
		*/
		/************************************************************************/
		void UnloadEntityExposedFields();

		/************************************************************************/
		/*!
		\brief
		Sets a flag to reload all scripts from each entity script component in next update
		*/
		/************************************************************************/
		void RefreshScripts() { m_attached = false; }

		/************************************************************************/
		/*!
		\brief
		Sets the values within the script instance to the modified one in the entity's
		script componenet
		*/
		/************************************************************************/
		void ModifyScriptValuesToPresetValues(void* script);

		/************************************************************************/
		/*!
		\brief
		Updates a value for a variable in a script via monoExposeBridge
		Also updates the script component

		\param scriptInstance
		void* of MonoObject script instance to update

		\param field
		Data of variable/field to update

		\param value
		New value to update variable/field to
		*/
		/************************************************************************/
		void UpdateScriptValue(void* scriptInstance, const ExposedField& field, const ExposedValue& value);

		/************************************************************************/
		/*!
		\brief
		Returns a script instance from an entity

		\param id
		ID of entity to search

		\param scriptName
		Script name of script to find

		\return
		void* of MonoObject script instance
		nullptr if there is no script found in itself
		*/
		/************************************************************************/
		bool FindScriptInEntity(Entity id, std::string scriptName, void** outScript);

		/************************************************************************/
		/*!
		\brief
		Returns a script instance from an entity, if it does not have it, it finds it in the children entity

		\param id
		ID of entity to search

		\param scriptName
		Script name of script to find

		\return
		void* of MonoObject script instance
		nullptr if there is no script found in either itself of children
		*/
		/************************************************************************/
		void* GetScriptInChild(Entity parentID, std::string scriptName);

		/************************************************************************/
		/*!
		\brief
		Returns a script instance from an entity, if it does not have it, it finds it in the parent entity

		\param id
		ID of entity to search

		\param scriptName
		Script name of script to find

		\return
		void* of MonoObject script instance
		nullptr if there is no script found in either itself or parent
		*/
		/************************************************************************/
		void* GetScriptInParent(Entity id, std::string scriptName);

	private:
		// Helper functions
		void Initialize();
		void Shutdown();

		// Create a new script instance attached to an entity by name;
		void* AttachScript(Entity e, std::string const& scriptClassName);
	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher{ nullptr };
		Coordinator& m_coord;

		std::unique_ptr<MonoScriptHost> m_monoHost;
		std::unique_ptr<ScriptInterface> m_scriptInterface;

		// Active script instances
		std::vector<void*> m_scriptInstances;// Map entity ID to script instances

		std::map<Entity, std::vector<void*>> m_entityScripts;
		//Tracks which script belongs to which entity
		std::map<void*, Entity> m_scriptOwner;
		std::map<void*, std::string> m_scriptNameMap;

		//Scripts that have not run onStart()
		std::list<void*> m_scriptsToStart;

		//Scripts that have not run onLoad()
		std::list<void*> m_scriptsToLoad;

		//This is used when scripts are being reloaded/loaded for the first time
		//Seperate from m_attached as something might be created mid update loop
		bool m_batchLoadScripts = false;

		bool debugMode = false;
		bool stepRequest = false;
		bool m_attached = false;

		size_t entityNum;

		MonoExposeBridge m_monoExposeBridge;
	};

}	// NightSnack