#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <memory>
#include <string>

#include "ECS/Coordinator.h"
#include "AppWindow/Window.h"
#include "AppWindow/Timer.h"
#include "ECS/ISystem.h"
#include "Systems/Application.h"

class CoreEngine {

public:

	void Initialize(int windowWidth, int windowHeight, std::string&& windowTitle);
	int Terminate();

	void Update();

	template <typename T>
	void AddSystem();
	template <typename T>
	void RemoveSystem(std::shared_ptr<T> system);

	Coordinator& GetCoord() { return m_coord; }

	bool ShouldClose() {  
		return m_coord.GetSystem<ApplicationSystem>().get()->ShouldClose();
	}

private:
	// To store all Systems;
	std::vector<std::shared_ptr<ISystem>> m_systems;

	// ECS Coordinator;
	Coordinator m_coord;
	int m_windowWidth{ 1600 }, m_windowHeight{ 900 };
	std::string m_windowTitle;
	// Application Window
	//std::unique_ptr<Window> m_coreWindow;
	// Timers
	Timer timer_;

};

template <typename T>
void CoreEngine::AddSystem() {
	// Ensure T derives from ISystem;
	static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");

	// Register the system in Coordinator (returns a shared_ptr<T>);
	auto systemPtr = m_coord.RegisterSystem<T>();

	// Add to CoreEngine's vector of systems as ISystem;
	m_systems.push_back(std::static_pointer_cast<ISystem>(systemPtr));
}

template <typename T>
void CoreEngine::RemoveSystem(std::shared_ptr<T> system) {
	// Shift unwanted Systems to the end of the vector, then uses erase() to remove them;
	m_systems.erase(std::remove(m_systems.begin(), m_systems.end(), system), m_systems.end());
}

#endif // CORE_HPP
