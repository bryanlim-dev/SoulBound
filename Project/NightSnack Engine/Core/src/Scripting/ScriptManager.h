#pragma once

#include "Core/ECS/Coordinator.h"
#include "ScriptInterface.h"

namespace NightSnack {




	class CORE_API ScriptManager : public ISystem {

	public:
		ScriptManager(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp } {

			// All entities;
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_scriptInterface = std::make_unique<Scripting::ScriptInterface>(m_coord);
			
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
		}

		void Update(int steps) override;

		Scripting::ScriptInterface* GetScriptInterface() {
			return m_scriptInterface.get();
		}

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const { return "ScriptManager"; }

		Coordinator& GetCoord() { return m_coord; }

	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher{ nullptr };
		Coordinator& m_coord;

		std::unique_ptr<Scripting::ScriptInterface> m_scriptInterface;

		bool debugMode = false;
		bool stepRequest = false;
	};


}	// NightSnack