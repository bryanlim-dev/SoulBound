#include "State.h"
#include <Systems/PlayerController.h>


namespace NightSnack
{

	bool StateBase::StateDecisionFuncionHelper::CheckPlayerIsLeftSide(Entity ownerID, Coordinator& coord)
	{
		Entity player = coord.GetSystem<PlayerControllerSystem>()->GetPlayer();
		if (coord.HasComponent<Transform>(player) && coord.HasComponent<Transform>(ownerID))
		{
			//Player x pos smaller than entity's
			if (coord.GetComponent<Transform>(player).pos.x <= coord.GetComponent<Transform>(ownerID).pos.x)
			{
				std::cout << "Player Left Side: Player.x : " << coord.GetComponent<Transform>(player).pos.x << ", Entity: " << coord.GetComponent<Transform>(ownerID).pos.x << std::endl;
				return true;
			}
		}

		return false;
	}

	bool StateBase::StateDecisionFuncionHelper::CheckPlayerIsRightSide(Entity ownerID, Coordinator& coord)
	{
		Entity player = coord.GetSystem<PlayerControllerSystem>()->GetPlayer();
		if (coord.HasComponent<Transform>(player) && coord.HasComponent<Transform>(ownerID))
		{
			if (coord.GetComponent<Transform>(player).pos.x >= coord.GetComponent<Transform>(ownerID).pos.x)
			{
				std::cout << "Player Right Side: Player.x : " << coord.GetComponent<Transform>(player).pos.x << ", Entity: " << coord.GetComponent<Transform>(ownerID).pos.x << std::endl;
				return true;
			}
		}

		return false;
	}

	bool StateBase::StateDecisionFuncionHelper::EntityIsColliding(Entity ownerID, Coordinator& coord)
	{
		if (coord.HasComponent<Collider>(ownerID) )
		{
			if (coord.GetComponent<Collider>(ownerID).colliding)
			{
				std::cout << "Entity Colliding!" << std::endl;
				return true;
			}
			//return coord.GetComponent<Collider>(ownerID).colliding;
		}

		return false;
	}

#pragma region CountDown State
	StateCountDown::StateCountDown(Entity id, std::string stateKeyName, Coordinator& coord, float maxTime)
		:StateBase(id, stateKeyName, coord, std::bind(&StateCountDown::CountdownUpdate, this, std::placeholders::_1, std::placeholders::_2)),
		m_maxTimer(maxTime), m_currTimer(maxTime)
	{
		//Can replace
		//std::bind(&StateCountDown::CountdownUpdate, this, std::placeholders::_1, std::placeholders::_2)
		//with
		//[this](float dt, Entity e) { CountdownUpdate(dt, e); }
	}

	void StateCountDown::Enter()
	{
		StateBase::Enter();

		m_currTimer = m_maxTimer;
	}

	void StateCountDown::Exit()
	{
		StateBase::Exit();
	}

	void StateCountDown::InsertEndCountdownState(std::shared_ptr<StateBase> nextState)
	{
		InsertStateTransition(nextState, std::bind(&StateCountDown::CountdownCheck, this, std::placeholders::_1));
	}

	void StateCountDown::CountdownUpdate(float dt, Entity id)
	{
		m_currTimer -= dt;
		///std::cout << "Counter: " << m_currTimer << std::endl;
	}

	bool StateCountDown::CountdownCheck(Entity id)
	{
		(void)id;
		//std::cout << "State adresss in CountdownUpdate: " << this << "\n";

		if (m_currTimer <= 0)
		{
			m_isExit = true;
			return true;
		}

		return false;
	}

	StateMovment::StateMovment(Entity id, std::string stateKeyName, Coordinator& coord, Vec2 moveSpeedAcc)
		:StateBase(id, stateKeyName, coord, (std::bind(&StateMovment::MovementUpdate, this))),
		m_Speed(moveSpeedAcc)
	{
		SetEnterFunction(std::bind(&StateMovment::MovementEnter, this));
		;
		SetExitFunction(std::bind(&StateMovment::MovementExit, this));
	}

	void StateMovment::MovementEnter()
	{
		m_coord.ModifyComponent<RigidBody>(m_entityID).vel = m_Speed;
	}

	void StateMovment::MovementUpdate()
	{
		//NOTE CHANGE THIS TO ACCEL
		//Vec2 newSpd = m_coord.GetComponent<RigidBody>(m_entityID).vel + m_Speed; //This one becomes a missle heheh
		Vec2 newSpd = m_Speed;
		m_coord.ModifyComponent<RigidBody>(m_entityID).vel = m_Speed;

		//Vec2 newAccel = m_coord.GetComponent<RigidBody>(m_entityID).accel + m_Speed;
		//m_coord.ModifyComponent<RigidBody>(m_entityID).accel = newAccel;
	}

	void StateMovment::MovementExit()
	{
		m_coord.ModifyComponent<RigidBody>(m_entityID).vel = Vec2(0,0);
	}
#pragma endregion
}