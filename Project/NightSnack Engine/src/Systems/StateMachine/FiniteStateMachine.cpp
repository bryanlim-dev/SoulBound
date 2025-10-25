#include "FiniteStateMachine.h"

namespace NightSnack
{
	FiniteStateMachine::FiniteStateMachine()
	{
	}

	FiniteStateMachine::~FiniteStateMachine()
	{
	}

	void FiniteStateMachine::Update(float dt, Coordinator& coord)
	{
		//Find and update each entity state machine
		std::list<Entity> removalList;

		for (Entity id : m_entityList)
		{
			if (coord.HasComponent<States>(id))
				UpdateEntityState(dt, coord, id, coord.ModifyComponent<States>(id));
			else
				removalList.push_back(id);
		}

		//Dont delete partway thru loop
		for (Entity id : removalList)
		{
			RemoveEntity(id);
		}
	}

	void FiniteStateMachine::AddEntity(Entity id)
	{
		m_entityList.push_back(id);
	}

	void FiniteStateMachine::RemoveEntity(Entity id)
	{
		m_entityList.remove(id);
	}


	void FiniteStateMachine::UpdateEntityState(float dt, Coordinator& coord, Entity id, States& states)
	{
		if (!states.currState)
		{
			std::cout << "Entity id: " << id << " has no current state!" << std::endl;
			return;
		}

		//First time using state machine, enter first state
		if (states.isFirstState)
		{
			states.currState->Enter();
			states.isFirstState = false;
			return;
		}

		//Not first time do regular state machine things
		states.currState->Update(dt);

		//Check if exiting
		if (states.currState->IsExit())
		{
			StateBase* nextState = states.currState->NextState();
			states.currState->Exit();

			states.currState = nextState;
			states.currState->Enter();
		}

		return;
	}


	std::string FiniteStateMachine::GetName() const
	{
		return "FiniteStateMachine";
	}

	Signature FiniteStateMachine::GetSignature() const
	{
		return m_systemSignature;
	}


	States FiniteStateMachine::CreateDebugTestStates(Entity entityId, Coordinator& coord)
	{
		std::map<std::string, std::shared_ptr<StateBase>> stateMap;

		std::string stateName = "CounterState";
		std::shared_ptr<StateCountDown> counterState = std::make_shared<StateCountDown>(StateCountDown(entityId, stateName, coord, 3));
		stateMap.insert({ stateName, counterState });

		stateName = "ChanceState";
		std::shared_ptr<StateBase> chanceState = std::make_shared<StateBase>(StateBase(entityId, stateName, coord));
		stateMap.insert({ stateName, chanceState });

		counterState->SetNextState(chanceState);
		counterState->InsertStateTransition(chanceState, std::bind(&StateCountDown::Countdown, counterState, std::placeholders::_1));
		chanceState->SetNextState(counterState);

		States state(stateMap, counterState.get());
		return state;
	}
}
