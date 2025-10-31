#include "StateComponents.h"
#include "../AllSystems.h"
#include <Core/ECS/Coordinator.h>

namespace NightSnack
{
	//Forward decleration
	class StateBase;

	namespace StateFunctionChecker
	{

		bool EntityCountdownComponentFinish(Entity ownerID, Coordinator& coord)
		{
			//Check if componet exists
			if (coord.GetComponent<States>(ownerID).currState->HasStateComponent("CooldownComponent"))
			{
				//Check curr timer <= 0
				StateComponentCountdown* cooldown = static_cast<StateComponentCountdown*>(coord.GetComponent<States>(ownerID).currState->GetStateComponent("CooldownComponent").get());
				if (cooldown->GetCurrTime() <= 0)
				{
					return true;
				}
			}

			return false;
		}

		bool CheckPlayerIsLeftSide(Entity ownerID, Coordinator& coord)
		{
			Entity player = coord.GetSystem<PlayerControllerSystem>()->GetPlayer();
			if (coord.HasComponent<Transform>(player) && coord.HasComponent<Transform>(ownerID))
			{
				//Player x pos smaller than entity's
				if (coord.GetComponent<Transform>(player).pos.x <= coord.GetComponent<Transform>(ownerID).pos.x)
				{
					//std::cout << "Player Left Side: Player.x : " << coord.GetComponent<Transform>(player).pos.x << ", Entity: " << coord.GetComponent<Transform>(ownerID).pos.x << std::endl;
					return true;
				}
			}

			return false;
		}

		bool CheckPlayerIsRightSide(Entity ownerID, Coordinator& coord)
		{
			Entity player = coord.GetSystem<PlayerControllerSystem>()->GetPlayer();
			if (coord.HasComponent<Transform>(player) && coord.HasComponent<Transform>(ownerID))
			{
				if (coord.GetComponent<Transform>(player).pos.x >= coord.GetComponent<Transform>(ownerID).pos.x)
				{
					//std::cout << "Player Right Side: Player.x : " << coord.GetComponent<Transform>(player).pos.x << ", Entity: " << coord.GetComponent<Transform>(ownerID).pos.x << std::endl;
					return true;
				}
			}

			return false;
		}

		bool EntityIsColliding(Entity ownerID, Coordinator& coord)
		{
			if (coord.HasComponent<Collider>(ownerID))
			{
				if (coord.GetComponent<Collider>(ownerID).colliding)
				{
					//std::cout << "Entity Colliding!" << std::endl;
					return true;
				}
				//return coord.GetComponent<Collider>(ownerID).colliding;
			}

			return false;
		}
	} //End of StateFunctionChecker namespace

	template<typename T>
	StateComponentVariable<T>::StateComponentVariable(T var) : m_variable(var)
	{
	}

	template<typename T>
	StateComponentVariable<T>::~StateComponentVariable()
	{
	}

	template<typename T>
	T StateComponentVariable<T>::GetVariable() const
	{
		return m_variable;
	}

	template<typename T>
	T& StateComponentVariable<T>::GetVariable()
	{
		return m_variable;
	}

	template<typename T>
	void StateComponentVariable<T>::SetVariable(T newVar)
	{
		m_variable = newVar;
	}


	StateComponentCountdown::StateComponentCountdown(float maxTime)
		: m_maxTime(maxTime), m_currTime(maxTime)
	{
	}

	StateComponentCountdown::~StateComponentCountdown()
	{
	}

	void StateComponentCountdown::OnEnter(Entity, Coordinator&)
	{
		m_currTime = m_maxTime;
	}
	void StateComponentCountdown::OnUpdate(float dt, Entity, Coordinator&)
	{
		m_currTime -= dt;

		//std::cout << "Time: " << m_currTime << std::endl;
		//Prevent underflow incase something cathastropic
		if (m_currTime < 0)
			m_currTime = 0;
	}

	void StateComponentCountdown::OnExit(Entity, Coordinator&)
	{
		m_currTime = 0;
	}

	float StateComponentCountdown::GetCurrTime() const
	{
		return m_currTime;
	}



	StateComponentMovement::StateComponentMovement(Vec2 spd) : m_spd(spd)
	{
	}

	StateComponentMovement::StateComponentMovement()
	{
	}

	void StateComponentMovement::OnEnter(Entity idOwner, Coordinator& coord)
	{
		coord.ModifyComponent<RigidBody>(idOwner).vel = m_spd;
	}
	void StateComponentMovement::OnUpdate(float dt, Entity idOwner, Coordinator& coord)
	{
		//NOTE CHANGE THIS TO ACCEL
		//Vec2 newSpd = m_coord.GetComponent<RigidBody>(m_entityID).vel + m_Speed; //This one becomes a missle heheh
		Vec2 newSpd = m_spd;
		coord.ModifyComponent<RigidBody>(idOwner).vel = newSpd;

		//Vec2 newAccel = m_coord.GetComponent<RigidBody>(m_entityID).accel + m_Speed;
		//m_coord.ModifyComponent<RigidBody>(m_entityID).accel = newAccel;
	}

	void StateComponentMovement::OnExit(Entity idOwner, Coordinator& coord)
	{
		coord.ModifyComponent<RigidBody>(idOwner).vel = Vec2(0, 0);
	}

	Vec2 StateComponentMovement::GetSpd() const
	{
		return m_spd;
	}



	std::shared_ptr<StateComponent> CreateStateComponent(std::string componentName, json& paramJson)
	{
		std::shared_ptr<StateComponent> component = nullptr;

		if (componentName == "countdown")
		{
			float maxTime = paramJson.get<float>();
			component = std::make_shared<StateComponentCountdown>(maxTime);
		}
		else if (componentName == "movment")
		{
			Vec2 speed;
			paramJson.at("fx").get_to(speed.x);
			paramJson.at("fy").get_to(speed.y);
			component = std::make_shared<StateComponentMovement>(speed);
		}

		return component;
	}
	TransitionFunction GetTransitionFunction(std::string transitonName)
	{
		if (transitonName == "IsPlayerLeftSide")
			return StateFunctionChecker::CheckPlayerIsLeftSide;
		else if (transitonName == "IsPlayerRightSide")
			return StateFunctionChecker::CheckPlayerIsRightSide;
		else if (transitonName == "IsCollided")
			return StateFunctionChecker::EntityIsColliding;
		else if (transitonName == "CountdownCompleted")
			return StateFunctionChecker::EntityCountdownComponentFinish;

		return TransitionFunction();
	}
	;

}

