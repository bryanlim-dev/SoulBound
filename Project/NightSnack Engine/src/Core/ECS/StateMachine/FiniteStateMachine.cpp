#include "FiniteStateMachine.h"

namespace NightSnack
{
	FiniteStateMachine::FiniteStateMachine(Entity entity)
		: entityID(entity)
	{
		DebugTestStates();
	}

	FiniteStateMachine::~FiniteStateMachine()
	{
		stateMap.clear();
	}

	void FiniteStateMachine::Update(float dt)
	{
		if (activeState)
		{
			activeState->Update(dt);

			//Check for state change
			if (activeState->IsExit())
			{
				std::shared_ptr<StateBase> nextState = activeState->NextState();
				activeState->Exit();

				activeState = nextState;
				activeState->Enter();
			}
		}
	}

	void FiniteStateMachine::DebugTestStates()
	{
		std::string stateName = "CounterState";
		std::shared_ptr<StateBase> counterState = std::make_shared<StateBase>(StateBase(entityID, stateName));
		stateMap.insert({ stateName, counterState });

		std::string stateName = "ChanceState";
		std::shared_ptr<StateBase> chanceState = std::make_shared<StateBase>(StateBase(entityID, stateName));
		stateMap.insert({ stateName, chanceState });

		counterState->SetNextState(chanceState);
		chanceState->SetNextState(counterState);
	}


}
