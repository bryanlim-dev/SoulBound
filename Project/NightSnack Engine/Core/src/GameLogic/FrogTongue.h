#pragma once
#include <MathLibrary/DistanceJoint2D.h>

namespace NightSnack {
	class FrogTongue
	{
	private:
		Entity m_owner = INVALID_ENTITY;
		Entity m_anchor = INVALID_ENTITY;
		Entity m_tongue = INVALID_ENTITY;

		DistanceJoint2D m_distanceJoint;

		float m_tongueLengthMax = 500.0f;
		float m_tongueWidth = 50.0f;

	public:
		FrogTongue(){}

		void Initalize(Entity owner) { m_owner = owner; }
		
		//Finds thed cloest attachtable surface/entity
		//Returns false if too far to attach or cant find
		bool Start(Coordinator& coord);
		void Update(Coordinator& coord);
		void End(Coordinator& coord);

		void UpdateTonguePos(Coordinator& coord);
	private:

	};
}