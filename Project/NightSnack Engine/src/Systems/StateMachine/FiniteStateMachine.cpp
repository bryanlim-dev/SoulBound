#include "FiniteStateMachine.h"
#include <Factory/EntityFactory.h>
#include "StateComponents.h"

namespace NightSnack
{
	FiniteStateMachine::FiniteStateMachine(Coordinator& coord) : m_coord(coord)
	{
		m_systemSignature.set(GetComponentTypeID<Transform>());

		//Entity test = m_coord.GetSystem<EntityFactory>()->Create("BananaBall");

 		//(FiniteStateMachine::CreateDebugTestStates(test, m_coord));
		//m_coord.ModifyComponent<Transform>(test).pos.x = 100;
		//m_coord.ModifyComponent<Transform>(test).pos.y = -225;

		//m_coord.ModifyComponent<Transform>(test).scale = {20, 20};
		//m_coord.ModifyComponent<Collider>(test).radius = 20;

		/*m_coord.GetSystem<FiniteStateMachine>()->*/
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
			std::weak_ptr<StateBase> nextState = states.currState->NextState().lock();
			states.currState->Exit();

			nextState.lock()->Enter();
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
		std::shared_ptr<StateBase> desicionState = std::make_shared<StateBase>(entityId, desisionName, coord);
		stateMap.insert({ desisionName, desicionState });


		std::string leftMovementName = "Left Movement State"; 
		std::shared_ptr<StateBase> movmenetLeftState = std::make_shared<StateBase>(entityId, leftMovementName, coord);
		stateMap.insert({ leftMovementName, movmenetLeftState });

		std::string rightMovementName = "Right Movmenet State";
		std::shared_ptr<StateBase> movemementRightState = std::make_shared<StateBase>(entityId, rightMovementName, coord);
		stateMap.insert({ rightMovementName, movemementRightState });


		std::string cooldownName = "CooldownState";
		std::shared_ptr<StateBase> stateCountdown = std::make_shared<StateBase>(entityId, cooldownName, coord);
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
