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
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include <Graphics/Texture.h>
#include <json.hpp>

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

			// Graphics event
			// Whenever a new texture has been created, update the AnimationClipsMap
			m_dispatcher->Subscribe<GraphicsEvent>([this](const GraphicsEvent& event) {
				switch (event.graphicsAction) {
				case GraphicsEvent::GraphicsAction::CreateTexture:
					////std::cout << "[ANIMATIONSYSTEM] received event\n";
					UpdateAnimationClipsMap();
					break;
				}
			});

			// Input event
			// Key press to change a single entity animation
			m_dispatcher->Subscribe<InputEvent>([this](const InputEvent& event) {
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::ChangeAnimation:
						if (!m_coord.IsPaused())
							ChangeAnimation();
						break;
					}
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
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
		Update the AnimationClipsMap whenever a new texture has been created. 
		Create AnimationClips from a created texture, based on its naming convention. 
		*/
		/************************************************************************/
		void UpdateAnimationClipsMap(); 

		/************************************************************************/
		/*!
		\brief
		Create AnimationClips. 
		Slice the texture into the respective number of Animation clips and Animation frames. 

		\param[in] textureName
		The name of the created texture.

		\param[in] tex
		Pointer to the texture image

		\param[in] cols
		The number of AnimationFrames to slice.

		\param[in] rows
		The number of AnimationClips to slice.
		*/
		/************************************************************************/
		void CreateAnimationClips(std::string textureName, int cols, int rows);

		/************************************************************************/
		/*!
		\brief
		Change the current animation clip of a single specified entity, to a random one, 
		but within the same spritesheet. 
		*/
		/************************************************************************/
		void ChangeAnimation();

		/************************************************************************/
		/*!
		\brief
		Save the current spritesheet to the disk using JSON

		\param[in] spritesheet
		The name of the spritesheet that is currently being saved
		*/
		/************************************************************************/
		void SaveAnimationAsset(const std::string& spritesheet);

		/************************************************************************/
		/*!
		\brief
		Load the current spritesheet from the disk using JSON

		\param[in] spritesheet
		The name of the spritesheet that is currently being loaded
		*/
		/************************************************************************/
		void LoadAnimationAsset(const std::string& spritesheet);

		/************************************************************************/
		/*!
		\brief
		Get the clip ID of an Animation Clip

		\param[in] spritesheet
		The name of the spritesheet that currently holds the Animation Clip you are finding

		\param[in] name
		The display name of the Animation Clip you are finding

		\return
		The clip ID of the Animation Clip (std::string)
		*/
		/************************************************************************/
		std::string FindAnimationClipIDByName(const std::string& spritesheet, const std::string& name);

		/************************************************************************/
		/*!
		\brief
		Helper function to play an Animation Clip

		\param[in] e
		The entity that will be playing the Animation Clip

		\param[in] restart
		Whether to restart the Animation Clip, if it was paused halfway only
		*/
		/************************************************************************/
		void PlayAnimation(Entity e, bool restart);

		/************************************************************************/
		/*!
		\brief
		Helper function to stop an Animation Clip

		\param[in] e
		The entity that is currently playing the Animation Clip

		\param[in] restart
		Whether to restart the Animation Clip, if it was paused halfway only
		*/
		/************************************************************************/
		void StopAnimation(Entity e);

		/************************************************************************/
		/*!
		\brief
		Getter function to whether the specific entity is currently playing an Animation Clip

		\param[in] e
		The entity that might be playing an Animation Clip

		\return
		Whether the specific entity is currently playing an Animation Clip
		*/
		/************************************************************************/
		bool IsPlayingAnimation(Entity e) const;

		/************************************************************************/
		/*!
		\brief
		Getter function to whether the specific entity is currently looping an Animation Clip

		\param[in] e
		The entity that might be looping an Animation Clip

		\return
		Whether the specific entity is currently looping an Animation Clip
		*/
		/************************************************************************/
		bool IsLoopingAnimation(Entity e) const;

		/************************************************************************/
		/*!
		\brief
		Setter function to set an Animation Clip's loop status 

		\param[in] e
		The entity holding the Animation clip

		\param[in] loop
		The updated loop status of the Animation Clip
		*/
		/************************************************************************/
		void SetLoopingAnimation(Entity e, bool loop);

		/************************************************************************/
		/*!
		\brief
		Set the current animation clip for an entity using the clip ID

		\param[in] animationClipName
		The name of the Animation Clip that should be the current one

		\param[in] e
		The specified entity
		*/
		/************************************************************************/
		void SetCurrentAnimationClipByID(const std::string& animationClipName, Entity e);

		/************************************************************************/
		/*!
		\brief
		Set the current animation clip for an entity using the clip's display name

		\param[in] e
		The specified entity

		\param[in] clipName
		The display name of the Animation Clip that should be the current one

		\return
		Success or not 
		*/
		/************************************************************************/
		bool SetCurrentAnimationClipByName(Entity e, const std::string& clipName);

		/************************************************************************/
		/*!
		\brief
		Getter function for the current animation clip display name

		\param[in] e
		The specified entity

		\return
		The display name of the current animation clip
		*/
		/************************************************************************/
		std::string GetCurrentAnimationClipName(Entity e) const;

		/************************************************************************/
		/*!
		\brief
		Get the names of all Animation Clip display names, owned by a specific entity.

		\param[in] e
		The specified entity

		\return
		A vector strings of all Animation Clip display names that the entity owns.
		*/
		/************************************************************************/
		std::vector<std::string> GetSpritesheetAnimationClipsNames(Entity e);

		/************************************************************************/
		/*!
		\brief
		Set the entity's animator component playback speed accordingly

		\param[in] e
		The specified entity

		\param[in] speed
		The updated playback speed
		*/
		/************************************************************************/
		void SetAnimationPlaybackSpeed(Entity e, float speed);

		/************************************************************************/
		/*!
		\brief
		Get the entity's animator component playback speed

		\param[in] e
		The specified entity

		\return
		THe entity's animator component playback speed
		*/
		/************************************************************************/
		float GetAnimationPlaybackSpeed(Entity e) const;

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

		// A single frame of an animation
		struct AnimationFrame {
			float U0{}, V0{}; // top left
			float U1{}, V1{}; // bottom right
			int timeDelay{}; // how long each frame will last in steps
		};

		// A single animation, comprising of many single frames
		struct AnimationClip {
			std::string name; // User can rename this
			std::vector<AnimationFrame> frames;
		};

		// Eg. - std::unordered_map<"banana", std::unordered_map<"banana_0", AnimationClip>> m_animationClips; 
		// "banana" - name of the sprite sheet
		// "banana_0" - name of one of the AnimationClip in "banana" sprite sheet
		std::unordered_map<std::string, std::unordered_map<std::string, AnimationClip>> m_animationClips;

	private:

		Signature m_systemSignature{};
		// Reference to Coordinator;
		Coordinator& m_coord;
		// System event dispatcher
		EventDispatcher* m_dispatcher = nullptr;
	};

	// AnimationFrame JSON helpers
	inline void to_json(json& j, const AnimationSystem::AnimationFrame& f) {
		j = { { "timeDelay", f.timeDelay } };
	}
	inline void from_json(const json& j, AnimationSystem::AnimationFrame& f) {
		j.at("timeDelay").get_to(f.timeDelay);
	}

	// AnimationClip JSON helpers
	inline void to_json(json& j, const AnimationSystem::AnimationClip& c) {
		j = { { "frames", c.frames } };
	}
	inline void from_json(const json& j, AnimationSystem::AnimationClip& c) {
		j.at("frames").get_to(c.frames);
	}

} // NightSnack