#include "FiniteStateMachine.h"

namespace NightSnack
{
	FiniteStateMachine::FiniteStateMachine(Coordinator& coord) : m_coord(coord)
	{
		Entity test = m_coord.CreateEntity();
		(FiniteStateMachine::CreateDebugTestStates(test, m_coord));
		/*m_coord.GetSystem<FiniteStateMachine>()->*/
		AddEntity(test);
	}

	FiniteStateMachine::~FiniteStateMachine()
	{
	}

	void FiniteStateMachine::Update(int steps)
	{
		for (int stepCount = 0; stepCount < steps; ++stepCount)
		{
			float dt = m_coord.GetSystem<ApplicationSystem>()->GetDT();

			//Find and update each entity state machine
			std::list<Entity> removalList;
			removalList.clear();

			for (Entity& id : m_entityList)
			{
				if (m_coord.HasComponent<States>(id))
					UpdateEntityState(dt, m_coord, id, m_coord.ModifyComponent<States>(id));
				else
					removalList.push_back(id);
			}

			//Dont delete partway thru loop
			for (Entity& id : removalList)
			{
				RemoveEntity(id);
			}
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
			std::shared_ptr<StateBase> nextState = states.currState->NextState();
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


	void FiniteStateMachine::CreateDebugTestStates(Entity entityId, Coordinator& coord)
	{
		std::map<std::string, std::shared_ptr<StateBase>> stateMap;

		//Do NOT use std::make_shared<StateCountDown>(StateCountDown(entityId, stateName, coord, 3))
		//Apprently that messes up the pointer and makes a new uninitalized one??

		std::string stateName = "CounterState";
		std::shared_ptr<StateCountDown> stateCountdownShared = std::make_shared<StateCountDown>(entityId, stateName, coord, 30);
		stateMap.insert({ stateName, stateCountdownShared });

		stateName = "ChanceState";
		std::shared_ptr<StateBase> chanceState = std::make_shared<StateBase>(entityId, stateName, coord, [](float, Entity) {
			});
		stateMap.insert({ stateName, chanceState });

		stateCountdownShared->SetNextState(chanceState);
		stateCountdownShared->InsertStateTransition(chanceState, std::bind(&StateCountDown::CountdownCheck, stateCountdownShared, std::placeholders::_1));
		chanceState->SetNextState(stateCountdownShared);
		//std::cout << "State adresss in debug states: " << stateCountdownShared << "\n";
		coord.AddComponent<States>(entityId, States(stateMap, stateCountdownShared));
		//std::cout << "State adresss in compionents: " << coord.GetComponent<States>(entityId).currState << "\n";
		//std::cout << "State adresss in commponent map: " << coord.ModifyComponent<States>(entityId).stateMap[stateName].get() << "\n";
		return;
	}
}
