/******************************************************************************
 * File:        SceneManager.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 60%)
 *				Bryan Lim Jun Jie (Secondary, 40%)
 *
 * Description:
 * This file declares the SceneManager class that performs scene managment 
 * such as level loading/saving, level caching when Play/Stop in Editor and 
 * GameUI interactions like button clicks.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include "Core/ECS/Coordinator.h"
#include "Systems/Application/IEvent.h"
#include "Systems/Application/Input.h"

#include "Factory/EntityFactory.h"
#include "Systems/AudioSystem.h"
#include "Scripting/ScriptManager.h"
#include "Systems/Graphics.h"

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

#include "Systems/ParticleSystem.h"
#include "Systems/CollectibleSystem.h"

namespace NightSnack::Scene {

	// Wrapper that contains a button's action as well as the callable to be invoked,
	// When the button is activated.
	class Button {
	public:
		/**
		 * @brief Constructs a button with an action and callable.
		 * @param a Button action enum value.
		 * @param func Function pointer to invoke when the button is triggered.
		 */
		Button(ButtonAction a, void(*func)(Coordinator& c, std::string const&, Entity))
			: m_action{ a }, m_callable{ func } {}
		Button(Button const& b) noexcept {
			m_action = b.m_action;
			m_callable = b.m_callable;
		}

		/**
		 * @brief Invokes the button's callable.
		 * @param c ECS coordinator reference.
		 * @param s String argument (e.g., level or menu name).
		 */
		void operator()(Coordinator& c, std::string const& s, Entity e) { m_callable(c, s, e); }

	private:
		ButtonAction m_action;
		void(*m_callable)(Coordinator& c, std::string const&, Entity);

	};

	// SceneManager system that handles level saving/loading, caching/restoring and
	// GameUI elements like button presses
	class SceneManager : public ISystem {
	public:
		/**
		 * @brief Constructs the SceneManager system.
		 * @param coord Reference to ECS Coordinator.
		 * @param disp Reference to the global EventDispatcher.
		 */
		SceneManager(Coordinator& coord, EventDispatcher& disp)
			: m_coord{ coord }, m_dispatcher{ &disp } , 
			m_scheduledLevelLoad(false),
			m_scheduledLevelName(""),
			m_levelLoadTimer(0.0f),
			m_scheduledExit(false),
			m_exitTimer(0.0f)
		{

			// set system signature
			m_systemSignature.set(GetComponentTypeID<ButtonInfo>());
			m_systemSignature.set(GetComponentTypeID<Transform>());
			m_systemSignature.set(GetComponentTypeID<Collider>());

			m_dispatcher->Subscribe<InputEvent>([this, &coord](const InputEvent& event) {
				// Handle ClickLMB outside the Pressed-only block
				if (event.inputAction == InputEvent::InputAction::ClickLMB) {
					if (m_coord.IsPaused()) return;
					if (event.pressed == InputEvent::InputActionState::Pressed) {
						m_mouseClick = true;
						m_mouseHeld = true;
					}
					else if (event.pressed == InputEvent::InputActionState::Released) {
						m_mouseHeld = false;
					}
					return;
				}

				if (event.inputAction == InputEvent::InputAction::ToggleCheat) {
					if (m_coord.IsPaused()) return;
					if (event.pressed == InputEvent::InputActionState::Pressed) {
						m_cheatcodeEnabled = !m_cheatcodeEnabled;
					}
				}
				else if (event.inputAction == InputEvent::InputAction::LevelTutorial) {
					if (m_coord.IsPaused()) return;
					if (event.pressed == InputEvent::InputActionState::Pressed) {
						if (m_cheatcodeEnabled)
							LoadLevel("LevelTutorial");
					}
				}
				else if (event.inputAction == InputEvent::InputAction::Level1) {
					if (m_coord.IsPaused()) return;
					if (event.pressed == InputEvent::InputActionState::Pressed) {
						if (m_cheatcodeEnabled)
							LoadLevel("Level1");
					}
				}
				else if (event.inputAction == InputEvent::InputAction::Level2) {
					if (m_coord.IsPaused()) return;
					if (event.pressed == InputEvent::InputActionState::Pressed) {
						if (m_cheatcodeEnabled)
							LoadLevel("Level2");
					}
				}
				else if (event.inputAction == InputEvent::InputAction::Level3) {
					if (m_coord.IsPaused()) return;
					if (event.pressed == InputEvent::InputActionState::Pressed) {
						if (m_cheatcodeEnabled)
							LoadLevel("Level3");
					}
				}
				
				if (event.pressed == InputEvent::InputActionState::Pressed ||
					event.pressed == InputEvent::InputActionState::Repeat)
				{
					// Menu keyboard / controller navigation
					// Fires for both m_menuOpen (pause/overlay menus) and main menu
					const bool anyMenuActive = m_menuOpen || m_mainMenuOpen;
					if (anyMenuActive)
					{
						const bool navUp   = (event.inputAction == InputEvent::InputAction::PlayerUp);
						const bool navDown = (event.inputAction == InputEvent::InputAction::PlayerDown);
						const bool navLeft = (event.inputAction == InputEvent::InputAction::PlayerMoveLeft);
						const bool navRight= (event.inputAction == InputEvent::InputAction::PlayerMoveRight);
						const bool confirm = (event.inputAction == InputEvent::InputAction::SkipDialogue);
						// B = back only when a menu overlay is open (NOT on main menu - nothing to go back to,
						// and B must still reach PlayerController as jump during main menu loading)
						const bool navBack = m_menuOpen &&
							(event.inputAction == InputEvent::InputAction::PlayerJumpAbility) &&
							(event.pressed    == InputEvent::InputActionState::Pressed);
						if (navBack)
						{
							// Find a Close/Back button and trigger via m_controllerClick so
							// CheckButtonCollisions handles physics unpause correctly
							for (Entity be : m_menuEntities)
							{
								if (!m_coord.HasComponent<ButtonInfo>(be)) continue;
								const ButtonInfo& bi = m_coord.GetComponent<ButtonInfo>(be);
								if (bi.action == ButtonAction::CLOSEMENU)
								{
									m_hoveredEntity = be;
									m_controllerClick = true;
									return;
								}
							}
							// No explicit close button found, call CloseMenu directly
							m_coord.GetSystem<Scene::SceneManager>()->CloseMenu();
							return;
						}

						if (navUp || navDown || navLeft || navRight || confirm)
						{
							// Buttons from menu entities (pause/overlay/mainmenu)
							const auto& sourceEntities = m_menuOpen ? m_menuEntities : m_entities;
							std::vector<Entity> navButtons;
							for (Entity e : sourceEntities)
							{
								if (!m_coord.HasComponent<ButtonInfo>(e) ||
									!m_coord.HasComponent<Transform>(e)  ||
									!m_coord.HasComponent<Collider>(e))   continue;
								const ButtonInfo& b = m_coord.GetComponent<ButtonInfo>(e);
								if (b.type == INVALID_BUTTON) continue;
								if (b.action == ButtonAction::VOLUMESLIDER ||
									b.action == ButtonAction::SFXSLIDER    ||
									b.action == ButtonAction::BGMSLIDER)   continue;
								navButtons.push_back(e);
							}
							// Sort top-to-bottom by Y
							std::sort(navButtons.begin(), navButtons.end(), [&](Entity a, Entity b) {
								return m_coord.GetComponent<Transform>(a).pos.y >
									   m_coord.GetComponent<Transform>(b).pos.y;
							});

							if (!navButtons.empty())
							{
								if (m_focusedButtonIndex < 0 || m_focusedButtonIndex >= (int)navButtons.size())
									m_focusedButtonIndex = 0;

								// Cooldown prevents joystick Repeat from moving too fast
								// Pressed always allowed; Repeat only when cooldown elapsed
								const bool isRepeat = (event.pressed == InputEvent::InputActionState::Repeat);
								const bool canNav = !isRepeat || (m_navCooldown <= 0.f);

								if (navUp && canNav)
								{
									m_focusedButtonIndex = (m_focusedButtonIndex - 1 + (int)navButtons.size()) % (int)navButtons.size();
									m_coord.GetSystem<AudioSystem>()->PlaySFX("buttonHover");
									m_navCooldown = 0.2f;
								}
								else if (navDown && canNav)
								{
									m_focusedButtonIndex = (m_focusedButtonIndex + 1) % (int)navButtons.size();
									m_coord.GetSystem<AudioSystem>()->PlaySFX("buttonHover");
									m_navCooldown = 0.2f;
								}

								// Highlight selected button
								Entity focused = navButtons[m_focusedButtonIndex];
								m_hoveredEntity = focused;
								m_coord.ModifyComponent<Renderable>(focused).materialName =
									m_coord.GetComponent<ButtonInfo>(focused).hoverTex;

								if (confirm)
								{
									m_hoveredEntity = focused;
									m_controllerClick = true;
									return;
								}
							}
						}

						// Slider navigation (left/right)
						if (navLeft || navRight)
						{
							auto adjustSlider = [&](Entity barEntity, Entity knobEntity)
							{
								if (barEntity == INVALID_ENTITY || knobEntity == INVALID_ENTITY) return;

								ButtonInfo& b = m_coord.ModifyComponent<ButtonInfo>(barEntity);
								constexpr float STEP = 0.05f;
								Transform const& barT = m_coord.GetComponent<Transform>(barEntity);
								float currentX = m_coord.GetComponent<Transform>(knobEntity).pos.x;
								float newX = std::clamp(currentX + (navRight ? STEP : -STEP) * barT.scale.x,
									b.sliderMinX, b.sliderMaxX);
								float t = (newX - b.sliderMinX) / (b.sliderMaxX - b.sliderMinX);

								m_coord.ModifyComponent<Transform>(knobEntity).pos.x = newX;

								if (b.action == ButtonAction::VOLUMESLIDER)
									m_coord.GetSystem<AudioSystem>()->SetMasterVolume(t);
								else if (b.action == ButtonAction::SFXSLIDER)
									m_coord.GetSystem<AudioSystem>()->SetSFXVolume(t);
								else if (b.action == ButtonAction::BGMSLIDER)
									m_coord.GetSystem<AudioSystem>()->SetBGMVolume(t);
							};

							adjustSlider(m_sliderBarEntity, m_sliderKnobEntity);
							adjustSlider(m_sfxBarEntity,    m_sfxKnobEntity);
							adjustSlider(m_bgmBarEntity,    m_bgmKnobEntity);
						}
						return;
					}

					switch (event.inputAction)
					{
					case InputEvent::InputAction::PauseGame:
						if (winScreenOn || m_coord.GetSystem<CameraSystem>()->GetCameraIsPanning() ||
							m_coord.GetSystem<DialogueSystem>()->IsActive()) {
							m_mouseClick = false;
							return;
						}
						// Find the PauseButton entity and trigger via m_controllerClick
						for (Entity pe : m_entities)
						{
							if (m_coord.HasComponent<Transform>(pe) &&
								m_coord.GetComponent<Transform>(pe).name == "PauseButton") {
								m_hoveredEntity = pe;
								m_controllerClick = true;
								break;
							}
						}
						break;
					case InputEvent::InputAction::RestartGame:
						m_restart = true;
						std::cerr << "[SceneManager] Queuing restart\n";
						break;
						//case InputEvent::InputAction::ChangeAnimation: // X
						//	CreateWinScreen();
						//	m_coord.GetSystem<AudioSystem>()->PlaySFXWithVolume("playerWin", 0.85f);
						//	m_coord.GetSystem<CollectibleSystem>()->ToggleCollectibleIcons(false);
						//	winScreenOn = true;
						//	break;
					}
				}
			});

			// Reads level files
			ReloadLevels();
			// Register buttons
			RegisterButtons();
		}
		
		/**
		 * @brief Per-frame update called in the core update loop.
		*/
		void Update() override;
		/**
		 * @brief Resets m_levelKeys and loads all json files in "GameData/Levels"
		 */
		void ReloadLevels();
		/**
		 * @brief Creates the scene camera from level data.
		 * @param levelJson JSON describing the camera configuration.
		 */
		void CreateSceneCamera(json const&);
		/**
		 * @brief Initializes the scene camera's max and min bounds.
		 */
		void InitSceneCameraBounds();

		/**
		 * @brief Reads camera animation keyframes from JSON.
		 * @param cameraJson JSON containing camera keyframe data.
		 */
		void ReadCameraKeyframes(json const&);
		/**
		 * @brief Serializes camera keyframes into JSON.
		 * @return JSON representation of camera keyframes.
		 */
		json WriteCameraKeyframes();
		/**
		 * @brief Gets all camera keyframes.
		 * @return Vector of pairs containing entity IDs and their corresponding keyframe JSON data.
		 */
		std::vector<std::pair<Entity, json>> GetCameraKeyframes() const;
		/**
		 * @brief Updates the camera keyframes.
		 * @param keyframes Vector of pairs containing entity IDs and their corresponding keyframe JSON data to update.
		 */
		void UpdateCameraKeyframes(std::vector<std::pair<Entity, json>> keyframes);

		/**
		 * @brief Creates a camera trigger entity.
		 * @param pos Position of the camera trigger.
		 * @param scale Scale of the camera trigger.
		 * @param bounds JSON object containing the camera trigger's bounds.
		 * @return The created camera trigger entity.
		 */
		Entity CreateCamTrigger(const Vec3& pos, const Vec2& scale, const json& bounds);
		/**
		 * @brief Deletes a camera trigger entity.
		 * @param camTriggerEntity The camera trigger entity to delete.
		 */
		void DeleteCamTrigger(Entity camTriggerEntity);

		// Spawn Overlay menu (Pause menu) (currently not in use)
		//void CreateOverlayMenu();
		//void RemoveOverlayMenu();
		
		//void SetOverlayMenu(std::string const& filename);
		
		/**
		 * @brief  Register button callable functions and store them in m_buttons
		 */
		void RegisterButtons();
		/**
		 * @brief Check for collision btw mouse and all button entities
		 */
		void CheckButtonCollisions();

		// Load Level - Sets up everything needed to spawn level
		// Calls factory to create entities
		// Sets up bgm and menus

		/**
		 * @brief Starts a new level
		 */
		void NewLevel();

		/**
		 * @brief Loads a level immediately.
		 * @param filename Level file to load.
		 */
		void LoadLevel(std::string const& filename);

		/**
		 * @brief Loads a level with fade-out and fade-in transitions.
		 * @param levelname Target level name.
		 * @param fadeOutDuration Fade-out duration in seconds.
		 * @param fadeInDuration Fade-in duration in seconds.
		 */
		void LoadLevelWithFade(std::string const& levelname, float fadeOutDuration, float fadeInDuration);
		/**
		 * @brief Schedules an application exit after a delay.
		 * @param delay Delay in seconds.
		 */
		void ScheduleExit(float delay);
		/**
		 * @brief Restarts the currently loaded level.
		 */
		void RestartLevel();
		/**
		 * @brief Saves the current level to a file.
		 * @param filename Level file to save to.
		 * @param overwrite Whether to overwrite existing file.
		 * @return True if save was successful, false otherwise.
		 */
		bool SaveLevel(std::string const& filename, bool overwrite = 0);

		// Play/Pause/Stop functionality

		/**
		 * @brief Caches the current level state.
		 */
		void CacheLevel();
		/**
		 * @brief Restores the previously cached level state.
		 */
		void RestoreLevel();

		/**
		 * @brief Opens a UI menu.
		 * @param menuname Menu identifier.
		 */
		void OpenMenu(std::string const& menuname);

		/**
		 * @brief Closes the currently open menu.
		 */
		void CloseMenu(bool o = false);

		/**
		 * @brief Closes all currently opened menus.
		 */
		void CloseAllMenus();

		/**
		 * @brief Checks if a menu is currently open.
		 * @return True if a menu is open.
		 */
		bool IsMenuOpen() const { return m_menuOpen; }

		// Getters
		const std::unordered_map<std::string, size_t>& GetAvailableLevels() const { return m_levelKeys; }

		bool GetCheatToggleActive() const { return m_cheatcodeEnabled; }

		// Required by ISystem base class;
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "SceneManager"; }

	private:
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;
		Coordinator& m_coord;
		
		json m_levelCache;
		//std::vector<std::pair<json, std::pair<json, Transform>>> m_levelCache;
		std::vector<std::pair<std::string, Transform>> m_overlayMenu;
		std::unordered_map<ButtonAction, Button> m_buttons;

		std::unordered_map<std::string, size_t> m_levelKeys;
		size_t m_currentLevelKey{};
		size_t m_previousLevelKey{};

		Entity m_sceneCam{ INVALID_ENTITY };
		float m_camDepth = 10.f;
		// Camera frames - Read from level.json
		std::map<Entity, json> m_camFrames;

		std::vector<Entity> m_overlayEntites;

		std::vector<std::string> m_currentLevelBGMs; // tracks BGMs loaded for current level

		std::vector<std::string> GetCurrentLevelBGMs() const { return m_currentLevelBGMs; }


		// Overlay menu
		bool m_gameLoaded = false;
		bool overlayOn = false;
		bool m_mouseClick = false;
		bool m_controllerClick = false;  // bypasses mouse position check in CheckButtonCollisions
		bool m_restart = false;
		bool m_mouseHeld = false;

		bool m_menuOpen = false;
		std::set<Entity> m_menuEntities;
		std::stack<std::string> m_menus;
		std::unordered_map<std::string, size_t> m_menuKeys; // Cache menu JSON keys
		
		bool m_scheduledLevelLoad;
		std::string m_scheduledLevelName;
		float m_levelLoadTimer;

		bool m_scheduledExit;
		float m_exitTimer;
		Entity m_player = INVALID_ENTITY;
		Entity m_camIndex = INVALID_ENTITY;
		Entity m_hoveredEntity = INVALID_ENTITY;
		int m_focusedButtonIndex{ 0 };  // keyboard/controller focused menu button index
		bool m_mainMenuOpen{ false };   // true when MainMenu level is active
		float m_navCooldown{ 0.f };     // seconds remaining before next nav move is allowed

		// Winzone
		Entity m_winCollider;
		std::string m_winScreen;
		bool winScreenOn = false;

		void CreateWinCollider(json const& level);
		json CacheWinCollider();
		void CreateWinScreen();

		//Cheatcodes
		bool m_cheatcodeEnabled = false;

		// entity for menu sliders
		Entity m_sliderBarEntity = INVALID_ENTITY;
		Entity m_sliderKnobEntity = INVALID_ENTITY;
		Entity m_sfxBarEntity = INVALID_ENTITY;
		Entity m_sfxKnobEntity = INVALID_ENTITY;
		Entity m_bgmBarEntity = INVALID_ENTITY;
		Entity m_bgmKnobEntity = INVALID_ENTITY;

		Entity GetSliderKnob() const { return m_sliderKnobEntity; }
		Entity GetSFXKnob() const { return m_sfxKnobEntity; }
		Entity GetBGMKnob() const { return m_bgmKnobEntity; }

	};

}	// NightSnack::Scene
