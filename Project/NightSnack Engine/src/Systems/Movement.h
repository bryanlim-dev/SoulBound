#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include <iostream>

#include "ECS/Entity.h"
#include "ECS/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"

class MovementSystem : public ISystem {

public:
	// Default constructor to set up SystemSignature;
  MovementSystem() {
    // Input all components you want access to...
    SystemSignature.set(GetComponentTypeID<Position>());
    SystemSignature.set(GetComponentTypeID<Velocity>());

		// Add more Components here if necessary...
  }

	void Update(double dt, Coordinator& Coord) override {

		// Iterate through all Entities this system can access;
    for (auto const& e : Entities) {

			// First obtain references to components of interest;
      auto& transform = Coord.GetComponent<Position>(e);
      auto& velocity = Coord.GetComponent<Velocity>(e);

			// Get more components here if necessary...

			// Do stuff that you want to do with the components;
      transform.x += velocity.dx * dt;
      transform.y += velocity.dy * dt;

      std::cout << "Entity " << e
          << " position: (" << transform.x << ", " << transform.y << ")\n";

			// Do more stuff here if necessary...
    }
	}
	Signature GetSignature() const override { return SystemSignature; }

	std::string GetName() const override { return "Movement"; }

private:
  Signature SystemSignature{};
};

// Declare and define more functions that will help you do stuff...

// Example:
// void SomeHelperFunction() {}
// ^ This function will then be called in either Update() or another function
//   that is called in Update();


#endif // MOVEMENT_HPP