#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "./Systems/Physics.h"

class MovementSystem : public ISystem {

public:
	// Default constructor to set up SystemSignature;
  MovementSystem(Coordinator& coord, EventDispatcher& dispatcher) {
    // Input all components you want access to...
    m_systemSignature.set(GetComponentTypeID<Position>());
    m_systemSignature.set(GetComponentTypeID<Velocity>());
    
    // Event dispatcher initialisations
    m_dispatcher = &dispatcher;
    m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
        if (event.pressed) {
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
	// Add more Components here if necessary...
  }

  void Update(float dt, Coordinator& coord) override {
      // Pause movement updates if in Physics debug mode
      if (debugMode && !stepRequest) {
          return;
      }

    // Iterate through all Entities this system can access;
    for (auto const& e : m_entities) {

	  // First obtain references to components of interest;
    auto& transform = coord.ModifyComponent<Position>(e);
    auto& velocity = coord.ModifyComponent<Velocity>(e);

	  // Get more components here if necessary...

	  // Do stuff that you want to do with the components;
    transform.x += velocity.dx * dt;
    transform.y += velocity.dy * dt;

	  // Do more stuff here if necessary...

	if (debugMode) stepRequest = false; // reset step request
    }
  }
Signature GetSignature() const override { return m_systemSignature; }

std::string GetName() const override { return "Movement"; }

private:
  Signature m_systemSignature{};
  EventDispatcher* m_dispatcher = nullptr;

  // DebugMode for Physics;
  bool debugMode = false;
  bool stepRequest = false;
};

// Declare and define more functions that will help you do stuff...

// Example:
// void SomeHelperFunction() {}
// ^ This function will then be called in either Update() or another function
//   that is called in Update();


#endif // MOVEMENT_HPP