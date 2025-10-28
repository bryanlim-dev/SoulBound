#include "State.h"


namespace NightSnack
{
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

	void StateCountDown::CountdownUpdate(float dt, Entity id)
	{
		m_currTimer -= dt;
		//std::cout << "Counter: " << m_currTimer << std::endl;
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
}