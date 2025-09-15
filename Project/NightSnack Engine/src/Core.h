#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <memory>
#include <string>

#include "ECS/Coordinator.h"
#include "AppWindow/Window.h"	
#include "ECS/ISystem.h"

class CoreEngine {

public:
	CoreEngine() = delete;
	CoreEngine(int WindowWidth, int WindowHeight, std::string WindowTitle)
		: CoreWindow(std::make_unique<Window>(WindowWidth, WindowHeight, WindowTitle)) {}

	void Initialize();
	int Terminate();

	void Update();

	template <typename T>
	void AddSystem();
	template <typename T>
	void RemoveSystem(std::shared_ptr<T> System);

	Coordinator& GetCoord() { return Coord; }

	bool ShouldClose() const { return CoreWindow->ShouldClose(); }

private:
	// To store all Systems;
	std::vector<std::shared_ptr<ISystem>> Systems;

	// ECS Coordinator;
	Coordinator Coord;
	// Application Window;
	std::unique_ptr<Window> CoreWindow;
};

template <typename T>
void CoreEngine::AddSystem() {
	// Ensure T derives from ISystem;
	static_assert(std::is_base_of<ISystem, T>::value, "T must derive from ISystem");

	// Register the system in Coordinator (returns a shared_ptr<T>);
	auto systemPtr = Coord.RegisterSystem<T>();

	// Add to CoreEngine's vector of systems as ISystem;
	Systems.push_back(std::static_pointer_cast<ISystem>(systemPtr));
}

template <typename T>
void CoreEngine::RemoveSystem(std::shared_ptr<T> System) {
	// Shift unwanted Systems to the end of the vector, then uses erase() to remove them;
	Systems.erase(std::remove(Systems.begin(), Systems.end(), system), Systems.end());
}

#endif // CORE_HPP
