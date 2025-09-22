#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <memory>
#include <string>

#include "ECS/Coordinator.h"
#include "ECS/ISystem.h"
#include "Systems/Application.h"

class CoreEngine {

public:

	void Initialize(int windowWidth, int windowHeight, std::string&& windowTitle);
	int Terminate();

	void Update();

	template <typename T, typename... Args>
	void AddSystem(Args&&... args);
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

#endif // CORE_HPP
