/******************************************************************************
 * File:        ResourceManager.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created class and logic
 *
 * Description:
 * This file contains the definition of the system class FiniteStateMachine.
 * It is NOT owned by a single entity and is instead a system that takes in 
 * the component States from entities and trigger 
 * the state functions (Upadte/Eneter/Exit/Switchhing,etc.)
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#include "FiniteStateMachine.h"
#include <Factory/EntityFactory.h>
#include "StateComponents.h"

namespace NightSnack
{
	FiniteStateMachine::FiniteStateMachine(Coordinator& coord) : m_coord(coord)
	{
		m_systemSignature.set(GetComponentTypeID<States>());

		//Entity test = m_coord.GetSystem<EntityFactory>()->Create("BananaBall");

 		//	(FiniteStateMachine::CreateDebugTestStates(test, m_coord));
		//m_coord.ModifyComponent<Transform>(test).pos.x = 100;
		//m_coord.ModifyComponent<Transform>(test).pos.y = -225;

		//m_coord.ModifyComponent<Transform>(test).scale = {20, 20};
		//m_coord.ModifyComponent<Collider>(test).radius = 20;

		///*m_coord.GetSystem<FiniteStateMachine>()->*/
		//AddEntity(test);
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

			for (auto const& e : m_entities) {
				if (m_coord.HasComponent<States>(e))
					UpdateEntityState(dt, e, m_coord.ModifyComponent<States>(e));
			}
		}
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
			states.currState->Enter(id, m_coord);
			states.isFirstState = false;
			return;
		}

		//Not first time do regular state machine things
		states.currState->Update(dt, id, m_coord);

		//Check if exiting
		if (states.currState->IsExit())
		{
			std::weak_ptr<StateBase> nextState = states.currState->NextState().lock();
			states.currState->Exit(id, m_coord);

			nextState.lock()->Enter(id, m_coord);
			states.currState = nextState.lock();
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

		//Descision State -> Movement (Left/Right) -> Countdown State -> Desicion

		std::string desisionName = "Descision State";
		std::shared_ptr<StateBase> desicionState = std::make_shared<StateBase>(desisionName);
		stateMap.insert({ desisionName, desicionState });


		std::string leftMovementName = "Left Movement State"; 
		std::shared_ptr<StateBase> movmenetLeftState = std::make_shared<StateBase>(leftMovementName);
		stateMap.insert({ leftMovementName, movmenetLeftState });

		std::string rightMovementName = "Right Movmenet State";
		std::shared_ptr<StateBase> movemementRightState = std::make_shared<StateBase>(rightMovementName);
		stateMap.insert({ rightMovementName, movemementRightState });


		std::string cooldownName = "CooldownState";
		std::shared_ptr<StateBase> stateCountdown = std::make_shared<StateBase>(cooldownName);
		stateMap.insert({ cooldownName, stateCountdown });


		//Now make the connections
		//Decision can move left or right
		desicionState->InsertStateTransition(movmenetLeftState, (StateFunctionChecker::CheckPlayerIsLeftSide)); //Player to the left, move left
		desicionState->InsertStateTransition(movemementRightState, (StateFunctionChecker::CheckPlayerIsRightSide)); //Player to the right, move right
		
		//Movement stops when colliding or when player is in wrong dir
		movmenetLeftState->InsertStateTransition(stateCountdown, (StateFunctionChecker::EntityIsColliding)); //Entity collided, wait
		movmenetLeftState->InsertStateComponent("LeftMovmentComponent", std::make_shared<StateComponentMovement>(Vec2{ -350, 0 }));
		//movmenetLeftState->InsertStateTransition(movemementRightState, (&StateBase::StateDecisionFuncionHelper::CheckPlayerIsRightSide)); //Player going other dir

		movemementRightState->InsertStateTransition(stateCountdown, (StateFunctionChecker::EntityIsColliding)); //Entity Collided wait
		movemementRightState->InsertStateComponent("RightMovmentComponent", std::make_shared<StateComponentMovement>(Vec2{ 350, 0 }));
		//movemementRightState->InsertStateTransition(movmenetLeftState, (&StateBase::StateDecisionFuncionHelper::CheckPlayerIsLeftSide)); //Pllayer going other dir

		//Countdown ends into decision
		stateCountdown->InsertStateTransition(desicionState, StateFunctionChecker::EntityCountdownComponentFinish);
		stateCountdown->InsertStateComponent("CooldownComponent", std::make_shared<StateComponentCountdown>(2.5));

		//stateCountdown->InsertStateComponent("Int", std::make_shared<int>(3));
		//int* x = (stateCountdown->GetStateComponent<int>("Int").get());
		//std::cout << "Int checker: " << x << std::endl;

		movmenetLeftState.reset();
		movemementRightState.reset();
		desicionState.reset();


		coord.AddComponent<States>(entityId, States(stateMap, stateCountdown));

		//std::string desisionName = "Descision State";
		//std::shared_ptr<StateBase> desicionState = std::make_shared<StateBase>(entityId, desisionName, coord);
		//stateMap.insert({ desisionName, desicionState });


		//std::string leftMovementName = "Left Movement State"; 
		//std::shared_ptr<StateMovment> movmenetLeftState = std::make_shared<StateMovment>(entityId, leftMovementName, coord, Vec2(-350,0));
		//stateMap.insert({ leftMovementName, movmenetLeftState });

		//std::string rightMovementName = "Right Movmenet State";
		//std::shared_ptr<StateMovment> movemementRightState = std::make_shared<StateMovment>(entityId, rightMovementName, coord, Vec2(350, 0));
		//stateMap.insert({ rightMovementName, movemementRightState });


		//std::string cooldownName = "CooldownState";
		//std::shared_ptr<StateCountDown> stateCountdown = std::make_shared<StateCountDown>(entityId, cooldownName, coord, 2);
		//stateMap.insert({ cooldownName, stateCountdown });


		////Now make the connections
		////Decision can move left or right
		//desicionState->InsertStateTransition(movmenetLeftState, (StateFunctionChecker::CheckPlayerIsLeftSide)); //Player to the left, move left
		//desicionState->InsertStateTransition(movemementRightState, (StateFunctionChecker::CheckPlayerIsRightSide)); //Player to the right, move right
		//
		////Movement stops when colliding or when player is in wrong dir
		//movmenetLeftState->InsertStateTransition(stateCountdown, (StateFunctionChecker::EntityIsColliding)); //Entity collided, wait
		////movmenetLeftState->InsertStateTransition(movemementRightState, (&StateBase::StateDecisionFuncionHelper::CheckPlayerIsRightSide)); //Player going other dir

		//movemementRightState->InsertStateTransition(stateCountdown, (StateFunctionChecker::EntityIsColliding)); //Entity Collided wait
		////movemementRightState->InsertStateTransition(movmenetLeftState, (&StateBase::StateDecisionFuncionHelper::CheckPlayerIsLeftSide)); //Pllayer going other dir

		////Countdown ends into decision
		//stateCountdown->InsertEndCountdownState(desicionState);



		//coord.AddComponent<States>(entityId, States(stateMap, desicionState));
		return;
	}
}
