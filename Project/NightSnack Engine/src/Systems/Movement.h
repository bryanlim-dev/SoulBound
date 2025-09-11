#ifndef MOVEMENT_HPP
#define MOVEMENT_HPP

#include "ECS/Types.h"
#include "Systems/ISystem.h"
#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include <iostream>

class MovementSystem : public ISystem {

public:
	void Update(double dt, Coordinator& Coord) override {
        for (auto const& e : Entities) {  // Entities is your system’s entity set
            auto& transform = Coord.GetComponent<Position>(e);
            auto& velocity = Coord.GetComponent<Velocity>(e);

            transform.x += velocity.dx * dt;
            transform.y += velocity.dy * dt;

            std::cout << "Entity " << e
                << " position: (" << transform.x << ", " << transform.y << ")\n";
        }
	}
};

#endif // MOVEMENT_HPP