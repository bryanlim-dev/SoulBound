/******************************************************************************
 * File:        AnimationSystem.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Animation System
 *
 * Description:
 * This file contains the declarations of the Animation System.
 * It initializes the AnimationClips and AnimationFrames of every texture loaded, based on its number of columns and rows.
 * It manages the updating of UV coordinates of the current AnimationClip, which is used by the Graphics System.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include <Graphics/Texture.h>

namespace NightSnack {

	class AnimationSystem : public ISystem {

	public:

		/************************************************************************/
		/*!
		\brief
		Constructor for the AnimationSystem
		Sets up the system signatures so that this system only operates on entities
		that contain components (Animator, Renderable)

		\param[in] coord
		Reference to the ECS Coordinator

		\param[in] dispatcher
		Reference to the event dispatcher used for Input Events
		*/
		/************************************************************************/
		AnimationSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
			m_systemSignature.set(GetComponentTypeID<Renderable>());
			m_systemSignature.set(GetComponentTypeID<Animator>());

			Init();

			// Dispatcher lambda
			m_dispatcher->Subscribe<GraphicsEvent>([this](const GraphicsEvent& event) {
				switch (event.graphicsAction) {
				case GraphicsEvent::GraphicsAction::CreateTexture:
					//std::cout << "[ANIMATIONSYSTEM] received event\n";
					UpdateAnimationClipsMap();
					break;
				}
			});
		}

		/************************************************************************/
		/*!
		\brief
		Initialise the map of animation clips, with its UV coordinates, with all the 
		stored pointer textures from Graphics System
		*/
		/************************************************************************/
		void Init();

		/************************************************************************/
		/*!
		\brief
		Main update called by the core.
		It updates the UV coordinates that will be used to render by the Graphics System, 
		based on which frame of the current animation clip is playing.

		\param[in] steps
		The number of steps in the simulation
		*/
		/************************************************************************/
		void Update(int steps) override;

		void UpdateAnimationClipsMap(); 

		void CreateAnimationClips(std::string textureName, Texture* tex, int cols, int rows);

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