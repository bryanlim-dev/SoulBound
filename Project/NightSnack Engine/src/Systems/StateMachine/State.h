#pragma once

#include <memory>
#include <string>
#include "Core/ECS/Entity.h"
#include <functional>
#include <iostream>


namespace NightSnack
{

	class StateBase
	{
	protected:
		std::shared_ptr<StateBase> m_nextState;
		Entity m_entityID;
		std::string m_stateKey; //Can double as the state name

		std::function<bool(Entity)> transitionFunction;

		bool m_isExit = false;

	public:
		StateBase(Entity id, std::string stateKeyName) 
			:m_entityID(id), m_stateKey(stateKeyName) {}

		virtual ~StateBase() {
			//Not needed now cause shared ptr but just incase
			if (m_nextState)
			{
				m_nextState.reset();
				m_nextState = nullptr;
			}
		}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			m_nextState = newNextState;
		}

		void SetTransitionCheck(std::function<bool(Entity)> transition)
		{
			transitionFunction = transition;
		}

		virtual void Enter() {
			m_isExit = false; 
			std::cout << "State Enter " << std::endl;
		}

		virtual void Update(float /*dt*/) {
			if (transitionFunction && transitionFunction(m_entityID))
			{
				m_isExit = true;
			}
		}

		virtual void Exit() { 
			m_isExit = false; 
			std::cout << "State Exit " << std::endl;
		}


		std::string StateKey() const
		{
			return m_stateKey;
		}

		std::shared_ptr<StateBase> NextState() const
		{
			return m_nextState;
		}

		bool IsExit() const
		{
			return m_isExit;
		}
	};

	class StateCountDown : public StateBase
	{
	private:
		float m_maxTimer;
		float m_currTimer;

	public:
		StateCountDown(Entity id, std::string stateKeyName, float maxTime);

		void Enter() override;

		void Update(float dt) override;

		void Exit() override;
	};

	class StateRandomChance : public StateBase
	{
	private:


	public:
		void Enter() override;

		void Update(float dt) override;

		void Exit() override;
	};

} //End of namesapce
