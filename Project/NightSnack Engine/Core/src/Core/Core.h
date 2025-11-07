/******************************************************************************
 * File:        Core.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * Declares CoreEngine class, application core with the main Init, Update and
 * terminate functions. Contains the ECS Coordinator, Event Dispatcher and all
 * registered systems are stored in m_systems.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <memory>
#include <string>

#include "ECS/Coordinator.h"
#include "ECS/ISystem.h"
#include "Systems/Application/Application.h"
#include "Systems/Application/IEvent.h"
#include "CrashLogger.h"

#include "ImportExport.h"	// DLL export

namespace NightSnack {

	class CORE_API CoreEngine {

	public:

		/************************************************************************/
		/*!
		\brief
		Wraps Initialize in a try catch in case of exceptions.
		*/
		/************************************************************************/
		void StartUp(std::string const& configFilePath);

		/************************************************************************/
		/*!
		\brief
		Registers all Components and Systems and initializes Coordinator.

		\param windowTitle
		Application config file path.
		*/
		/************************************************************************/
		void Initialize(std::string const& configFilePath);

		/************************************************************************/
		/*!
		\brief
		Does not currently do anything, will move all cleanup functions inside
		here for clean main function.
		*/
		/************************************************************************/
		int Terminate();

		/************************************************************************/
		/*!
		\brief
		Updates all systems in m_systems.
		*/
		/************************************************************************/
		void Update();

		/************************************************************************/
		/*!
		\brief
		Calls Update() in a while loop until application ShouldClose.
		*/
		/************************************************************************/
		void Run();

		/************************************************************************/
		/*!
		\brief
		Registers a system and adds it into m_systems container.
		Arguments are forwarded to individual system constructors.
		*/
		/************************************************************************/
		template <typename T, typename... Args>
		void AddSystem(Args&&... args);

		/************************************************************************/
		/*!
		\brief
		Removes a system from the core engine.
		*/
		/************************************************************************/
		template <typename T>
		void RemoveSystem(std::shared_ptr<T> system);

		/************************************************************************/
		/*!
		\brief
		Obtains a reference to the Coordinator.
		*/
		/************************************************************************/
		Coordinator& GetCoord() { return m_coord; }

		/************************************************************************/
		/*!
		\brief
		Obtains a reference to the Event Dispatcher.
		*/
		/************************************************************************/
		EventDispatcher& GetEventDispatcher() { return core_dispatcher; }

		/************************************************************************/
		/*!
		\brief
		Checks if the application window should close.

		\return
		Returns true if the application should close, false otherwise.
		*/
		/************************************************************************/
		bool ShouldClose() {
			return m_coord.GetSystem<ApplicationSystem>().get()->ShouldClose();
		}

	private:
		// To store all Systems;
		std::vector<std::shared_ptr<ISystem>> m_systems;

		// ECS Coordinator;
		Coordinator m_coord;

		// Event Dispatcher;
		EventDispatcher core_dispatcher;

		bool m_paused = false;
	};

	template <typename T, typename... Args>
	void CoreEngine::AddSystem(Args&&... args) {
		// Ensure T derives from ISystem;
		static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");

		// Register the system in Coordinator (returns a shared_ptr<T>);
		auto systemPtr = m_coord.RegisterSystem<T>(std::forward<Args>(args)...);

		// Add to CoreEngine's vector of systems as ISystem;
		m_systems.push_back(std::static_pointer_cast<ISystem>(systemPtr));
	}

	template <typename T>
	void CoreEngine::RemoveSystem(std::shared_ptr<T> system) {
		// Shift unwanted Systems to the end of the vector, then uses erase() to remove them;
		m_systems.erase(std::remove(m_systems.begin(), m_systems.end(), system), m_systems.end());
	}

}	// NightSnack

#endif // CORE_HPP
