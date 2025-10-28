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

		//Function returns nothing an takes in a EntityID and float (dt)
		std::function<void (Entity, Coordinator&)> m_enterFunction;
		std::function<void (float, Entity)> m_updateFunction;
		std::function<void (Entity, Coordinator&)> m_exitFunction;

		std::shared_ptr<StateBase> m_nextState = nullptr;
		Entity m_entityID;
		Coordinator& m_coord;

		std::string m_stateKey; //Can double as the state name

		bool m_isExit = false;

	public:
		StateBase() = delete;

		StateBase(Entity id, std::string stateKeyName, Coordinator& coord, std::function<void(float, Entity)> updateFunction)
			:m_entityID(id), m_stateKey(stateKeyName), m_coord(coord){
			m_updateFunction = updateFunction;
			//std::cout << "State adresss in constructor: " << this << "\n";
		}

		virtual ~StateBase() {
			if (m_nextState)
			{
				m_nextState = nullptr;
			}
		}

		void SetEnterFunction(std::function<void(float, Coordinator&)> enterFunc)
		{
			m_enterFunction = enterFunc;
		}

		void SetExitFunction(std::function<void(float, Coordinator&)> exitFunc)
		{
			m_exitFunction = exitFunc;
		}

		void InsertStateTransition(std::shared_ptr<StateBase> state, std::function<bool(Entity)>transitionn)
		{
			m_transtitionStatesList.push_back({ state,transitionn });
		}

		void SetNextState(std::shared_ptr<StateBase> newNextState) {
			m_nextState = newNextState;
		}

		virtual void Enter() {
			m_isExit = false; 
			std::cout << "ID: " << m_entityID << ", State Enter: " << m_stateKey << std::endl;

			if (m_enterFunction)
				m_enterFunction(m_entityID, m_coord);
		}

		virtual void Update(float dt) {
			//Call update function
			m_updateFunction(dt, m_entityID);

			for (auto& stateFuncPair : m_transtitionStatesList)
			{
				if (stateFuncPair.second && stateFuncPair.second(m_entityID))
				{
					m_nextState = stateFuncPair.first;
					m_isExit = true;
					return;
				}
			}
		}

		virtual void Exit() { 
			m_isExit = false; 
			std::cout << "ID: " << m_entityID << ", State Exit: " << m_stateKey << std::endl;

			if (m_exitFunction)
				m_exitFunction(m_entityID, m_coord);
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

		struct StateDecisionFuncionHelper
		{
			//Insert update functions here
			//bool CheeckPlayerLeft(Entity ownerID, Coordinator& coord);
		};
	};

	class StateCountDown : public StateBase
	{
	private:
		float m_maxTimer;
		float m_currTimer;

	public:
		StateCountDown(Entity id, std::string stateKeyName, Coordinator& coord, float maxTime);

		void Enter() override;

		//Use default
		//void Update(float dt) override;

		void Exit() override;

		void CountdownUpdate(float dt, Entity id);

		bool CountdownCheck(Entity id);
	};

	class StateMovment : public StateBase
	{
	private:
		Vec2 m_Speed;

	public:
		StateMovment(Entity id, std::string stateKeyName, Coordinator& coord, Vec2 moveSpeedAcc);

		void MovementEnter();
		void MovementExit();
	};

} //End of namesapce

#endif 