#pragma once

#include <memory>
#include <string>
#include "Core/ECS/Entity.h"
#include <functional>


namespace NightSnack
{

	class StateBase
	{
	private:
		std::shared_ptr<StateBase> nextState;
		Entity entityID;
		std::string stateKey; //Can double as the state name

		std::function<bool(Entity)> transitionFunction;

		bool isExit = false;

	public:
		StateBase(Entity id, std::string stateKeyName) 
			:entityID(id), stateKey(stateKeyName) {}

		virtual ~StateBase() {
			//Not needed now cause shared ptr but just incase
			if (nextState)
			{
				nextState.reset();
				nextState = nullptr;
			}
		}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			nextState = newNextState;
		}

		void SetTransitionCheck(std::function<bool(Entity)> transition)
		{
			transitionFunction = transition;
		}

		virtual void Enter() { isExit = false; }

		virtual void Update(float dt) {
			if (transitionFunction(entityID))
			{
				isExit = true;
			}

		}
		virtual void Exit() { isExit = false; }


		std::string StateKey() const
		{
			return stateKey;
		}

		std::shared_ptr<StateBase> NextState() const
		{
			return nextState;
		}

		bool IsExit() const
		{
			return isExit;
		}
	};
}