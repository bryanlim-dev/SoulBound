#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h""
#include <Graphics/Texture.h>

namespace NightSnack {

	class AnimationSystem : public ISystem {

	public:

		AnimationSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
			m_systemSignature.set(GetComponentTypeID<Renderable>());
			m_systemSignature.set(GetComponentTypeID<Animator>());

			Init();

			// Dispatcher lambda
			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				if (m_entities.empty())
					return;

				});
		}

		void Init();

		/************************************************************************/
		/*!
		\brief
		Main update called by the core

		\param[in] dt
		Delta time

		\param[in] coord
		Reference to the ECS Coordinator
		*/
		/************************************************************************/
		void Update(int steps) override;

		/************************************************************************/
		/*!
		\brief
		Get the system's signature.
		Required by ISystem inheritance

		\return
		Signature (set of required components)
		*/
		/************************************************************************/
		Signature GetSignature() const override { return m_systemSignature; }

		/************************************************************************/
		/*!
		\brief
		Gets the name of this system, for debugging purposes
		Required by ISystem inheritance

		\return
		Name of the system
		*/
		/************************************************************************/
		std::string GetName() const { return "AnimationSystem"; }

	private:

		// A single frame of an animation
		struct AnimationFrame {
			float U0{}, V0{}; // top left
			float U1{}, V1{}; // bottom right
			float timeDelay{}; // how long each frame will last
		};

		// A single animation, comprising of many single frames
		struct AnimationClip {
			Texture* texture = nullptr;
			std::vector<AnimationFrame> frames;
		};

		// Eg. - std::unordered_map<"banana", std::unordered_map<"banana_0", AnimationClip>> m_animationClips; 
		// "banana" - name of the sprite sheet
		// "banana_0" - name of one of the AnimationClip in "banana" sprite sheet
		std::unordered_map<std::string, std::unordered_map<std::string, AnimationClip>> m_animationClips;

		Signature m_systemSignature{};
		// Reference to Coordinator;
		Coordinator& m_coord;
		// System event dispatcher
		EventDispatcher* m_dispatcher = nullptr;
	};

} // NightSnack