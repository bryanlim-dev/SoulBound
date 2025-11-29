/******************************************************************************
 * File:        ScriptManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
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
		}
		// Destructor that calls shutdown member function;
		~ScriptManager() { Shutdown(); }
		
		// Main Update function;
		void Update() override;

		//Creates a new script and modifies the Script component for the entity
		bool CreateScript(Entity e, std::string scriptName);
		// Remove all scripts from entity;
		void RemoveAllScripts(Entity e);
		// Remove the first matching script from entity;
		void RemoveScript(Entity e, std::string scriptName);

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const { return "ScriptManager"; }

		std::vector<void*> GetEntityScripts(Entity e);
		std::string GetScriptName(void* script);

		MonoExposeBridge& GetMonoExposeBridge();
		void LoadEntityExposedFields(Entity e);
		void UnloadEntityExposedFields();

		void RefreshScripts() { m_attached = false; }

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

		// Map entity ID to script instance
		std::map<Entity, std::vector<void*>> m_entityScripts;
		std::map<void*, std::string> m_scriptNameMap;

		bool debugMode = false;
		bool stepRequest = false;
		bool m_attached = false;

		size_t entityNum;

		MonoExposeBridge m_monoExposeBridge;
	};

}	// NightSnack