#pragma once
#include "ECS/Coordinator.h"
#include "ECS/Types.h"

class ISystem {

public:
	virtual ~ISystem() = default;
	
	// Entities that the system can access;
	std::set<Entity> Entities;
	// Every system must define an Update function;
	virtual void Update(double dt, Coordinator& Coord) = 0;

	void SetCoordinator(std::shared_ptr<Coordinator> crd) { Coord = crd; }

private:
	std::shared_ptr<Coordinator> Coord{};
};