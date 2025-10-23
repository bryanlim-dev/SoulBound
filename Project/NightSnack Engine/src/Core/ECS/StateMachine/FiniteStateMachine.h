#pragma once
#include "State.h"
#include <map>

namespace NightSnack
{
	class FiniteStateMachine
	{
	private:
		Entity entityID;
		std::map<std::string, std::shared_ptr<StateBase>> stateMap;

		std::shared_ptr<StateBase> activeState;

	public:
		FiniteStateMachine(Entity entity);
		~FiniteStateMachine();

		void Update(float dt);

		void DebugTestStates();
	};
}