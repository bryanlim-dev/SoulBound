#pragma once

#ifndef State
#define State

#include <memory>
#include <string>
#include <functional>
#include <iostream>
#include "../../Core/ECS/Coordinator.h"
//#include "../../Core/ECS/Entity.h"




namespace NightSnack
{

	class StateBase
	{
	protected:
		std::list<std::pair<std::shared_ptr<StateBase>, std::function<bool(Entity)>>> m_transtitionStatesList;
		StateBase* m_nextState = nullptr;
		Entity m_entityID;
		Coordinator& m_coord;

		std::string m_stateKey; //Can double as the state name

		bool m_isExit = false;

	public:
		StateBase() = delete;

		StateBase(Entity id, std::string stateKeyName, Coordinator& coord)
			:m_entityID(id), m_stateKey(stateKeyName), m_coord(coord) {}

		virtual ~StateBase() {
			if (m_nextState)
			{
				m_nextState = nullptr;
			}
		}

		void InsertStateTransition(std::shared_ptr<StateBase> state, std::function<bool(Entity)>transitionn)
		{
			m_transtitionStatesList.push_back({ state,transitionn });
		}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			m_nextState = newNextState.get();
		}

		virtual void Enter() {
			m_isExit = false; 
			std::cout << "ID: " << m_entityID << ", State Enter: " << m_stateKey << std::endl;
		}

		virtual void Update(float /*dt*/) {
			for (auto stateFuncPair : m_transtitionStatesList)
			{
				if (stateFuncPair.second && stateFuncPair.second(m_entityID))
				{
					m_nextState = stateFuncPair.first.get();
					m_isExit = true;
					return;
				}
			}
		}

		virtual void Exit() { 
			m_isExit = false; 
			std::cout << "ID: " << m_entityID << ", State Exit: " << m_stateKey << std::endl;
		}


		std::string StateKey() const
		{
			return m_stateKey;
		}

		StateBase* NextState() const
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
		StateCountDown(Entity id, std::string stateKeyName, Coordinator& coord, float maxTime);

		void Enter() override;

		void Update(float dt) override;

		void Exit() override;

		bool Countdown(Entity id);
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

#endif 