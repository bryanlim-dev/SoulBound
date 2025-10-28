#include "FiniteStateMachine.h"
#include <Factory/EntityFactory.h>

namespace NightSnack
{
	FiniteStateMachine::FiniteStateMachine(Coordinator& coord) : m_coord(coord)
	{
		m_systemSignature.set(GetComponentTypeID<Transform>());

		Entity test = m_coord.GetSystem<EntityFactory>()->Create("BananaBall");

 		(FiniteStateMachine::CreateDebugTestStates(test, m_coord));
		m_coord.ModifyComponent<Transform>(test).pos.x = 100;
		m_coord.ModifyComponent<Transform>(test).pos.y = 250;

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
					UpdateEntityState(dt, id, m_coord.ModifyComponent<States>(id));
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


	void FiniteStateMachine::UpdateEntityState(float dt, Entity id, States& states)
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

		std::string stateName = "CounterState";
		std::shared_ptr<StateCountDown> stateCountdownShared = std::make_shared<StateCountDown>(entityId, stateName, coord, 5);
		stateMap.insert({ stateName, stateCountdownShared });

		stateName = "MoveState";
		//std::shared_ptr<StateBase> moveState = std::make_shared<StateBase>(entityId, stateName, coord, [](float, Entity) {
		//	});
		std::shared_ptr<StateMovment> moveState = std::make_shared<StateMovment>(entityId, stateName, coord, Vec2(100, 0));
		stateMap.insert({ stateName, moveState});

		stateCountdownShared->SetNextState(moveState);
		stateCountdownShared->InsertStateTransition(moveState, std::bind(&StateCountDown::CountdownCheck, stateCountdownShared, std::placeholders::_1));
		moveState->SetNextState(stateCountdownShared);

		coord.AddComponent<States>(entityId, States(stateMap, stateCountdownShared));
		return;
	}
}
