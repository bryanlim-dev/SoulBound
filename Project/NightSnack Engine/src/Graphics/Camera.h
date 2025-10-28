/******************************************************************************
 * File:        Camera.h
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Primary, 100%) -
 *
 * Description:

 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/ISystem.h"
#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Component.h"
#include "Systems/Application/IEvent.h"

namespace NightSnack {

	class CameraSystem : public ISystem {
	public:

		CameraSystem(Coordinator& coordinator, EventDispatcher& dispatcher);
		~CameraSystem() = default;

		void Update(int steps) override;
		Signature GetSignature() const override;
		std::string GetName() const override;

	private:
		Coordinator* m_coord;
		EventDispatcher* m_dispatcher;

		Entity m_mainCamera;
		Entity m_player;

		Entity CameraComponent(Coordinator& coord);

		Entity FindPlayer(Coordinator& coord);
	};

}