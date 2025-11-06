#pragma once

#include "Core/ECS/Coordinator.h"
#include "ScriptInterface.h"
#include "Scripting/MonoScriptHost.h"

#include "ImportExport.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace NightSnack::Scripting {

	class CORE_API ScriptManager : public ISystem {

	public:
		ScriptManager(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp } {

			// Event dispatcher initialisations
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::PhysicsDebugMode:
						debugMode = !debugMode;
						break;
					case InputEvent::InputAction::PhysicsDebugStep:
						stepRequest = true;
						break;
					}
				}
				});

			Initialize();
		}
		~ScriptManager() { Shutdown(); }
		
		// Main Update function;
		void Update(int steps) override;

		// Create a script instance attached to an entity
		void* AttachScript(Entity e, std::string const& scriptClassName);
		// Remove script from entity
		void RemoveScript(Entity e);

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const { return "ScriptManager"; }

	private:
		// Helper functions
		void Initialize();
		void Shutdown();

	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher{ nullptr };
		Coordinator& m_coord;

		std::unique_ptr<MonoScriptHost> m_monoHost;
		std::unique_ptr<ScriptInterface> m_scriptInterface;

		// Map entity ID to script instance
		std::unordered_map<Entity, void*> m_entityScripts;

		bool debugMode = false;
		bool stepRequest = false;
	};

}	// NightSnack