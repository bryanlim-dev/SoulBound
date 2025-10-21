#pragma once

#include <memory>
#include "Core/ECS/Entity.h"


namespace NightSnack
{

	class StateBase
	{
	private:
		//A weak ptr to the next state
		std::weak_ptr<StateBase> nextState;
		Entity entityID;

		bool isExit = false;

	public:
		StateBase(Entity id) :entityID(id) {}
		virtual ~StateBase() {}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			nextState = newNextState;
		}

		virtual void Enter() {}
		virtual void Update() {}
		virtual void Exit() { isExit = false; }

	};
}