#pragma once

#include <memory>
#include <string>
#include "Core/ECS/Entity.h"


namespace NightSnack
{

	class StateBase
	{
	private:
		std::shared_ptr<StateBase> nextState;
		Entity entityID;
		std::string stateName;

		bool isExit = false;

	public:
		StateBase(Entity id) :entityID(id) {}
		virtual ~StateBase() {
			if (nextState)
			{
				nextState.reset();
				nextState = nullptr;
			}
		}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			nextState = newNextState;
		}

		virtual void Enter() { isExit = false; }
		virtual void Update() {}
		virtual void Exit() { isExit = false; }


		std::string StateName() const
		{
			return stateName;
		}

		std::shared_ptr<StateBase> NextState() const
		{
			return nextState;
		}
	};
}