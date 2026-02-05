/******************************************************************************
 * File:        Imgui.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, 88%) - Implemented Editor using ImGui System.
 *				Included various panels: Hierarchy, Inspector, Scene, Options, Assets.
 *				Added docking, menu bar, and theme styling.
 *				Chewn Thing Kwan (Secondary, 12%) - Created base ImGui System
 *
 * Description: 
 * This file contains the definition of ImguiSystem which handles the 
 * ImGui user interface for the editor.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include "Panels/AllPanels.h"

#include "Systems/Application/Application.h"
#include "GameLogic/SceneManager.h"

namespace NightSnack {

	class ImguiSystem : public ISystem {

	public:
		/************************************************************************/
		/*!
		\brief
			Creates imgui context and initializes OpenGL for imgui panel.
		*/
		/************************************************************************/
		ImguiSystem(Coordinator& coord, EventDispatcher& dispatcher);

		/************************************************************************/
		/*!
		\brief
			Destroys imgui context and shuts down OpenGL and GLFW for imgui.
		*/
		/************************************************************************/
		~ImguiSystem();

		/************************************************************************/
		/*!
		\brief
			Initializes ImGui context and configures settings.
		*/
		/************************************************************************/
		void Init();

		/************************************************************************/
		/*!
		\brief
			Called in Core update, sets up and draws UI panels.

		\param steps
			Number of steps to update.
		*/
		/************************************************************************/
		void Update() override;

		/************************************************************************/
		/*!
		\brief
			Destroys and resets Imgui & all panels.
		*/
		/************************************************************************/
		void ResetAll();

		/************************************************************************/
		/*!
		\brief
			Resets the game to initial state.
		*/
		/************************************************************************/
		void ResetGame();

		ScenePanel& GetScenePanel() { return *m_scenePanel; }

		// Required by ISystem base class.
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "ImguiSystem"; }

	private:
		Coordinator& m_coord;
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;

		// Editor state
		bool m_toggleEditor = false;
		bool m_isPlaying = false;

		Tilemap m_tilemap;
		UndoSystem* m_undoSystem = nullptr;

		// Panels and shared data
		std::shared_ptr<std::unordered_map<Entity, std::string>> m_uiNames;
		std::shared_ptr<Entity> m_selectedEntity;
		std::unique_ptr<HierarchyPanel> m_hierarchyPanel;
		std::unique_ptr<InspectorPanel> m_inspectorPanel;
		std::unique_ptr<ScenePanel> m_scenePanel;
		std::unique_ptr<OptionsPanel> m_optionsPanel;
		std::unique_ptr<AssetsPanel> m_assetsPanel;
		std::unique_ptr<AnimationPanel> m_animationPanel;

		// Panel visibility toggles
		bool m_showHierarchy = true;
		bool m_showInspector = true;
		bool m_showScene = true;
		bool m_showOptions = true;
		bool m_showAssets = true;
		bool m_showAnimation = true;

		/************************************************************************/
		/*!
		\brief
			Sets up the main dockspace for the editor panels.
		*/
		/************************************************************************/
		void SetupDockspace();

		/************************************************************************/
		/*!
		\brief
			Draws the main menu bar for the editor.
		*/
		/************************************************************************/
		void DrawMenuBar();
		
		/************************************************************************/
		/*!
		\brief
			Enables or disables the editor UI.

		\param[in] enabled
			- Switch to enable or disable the editor.
		*/
		/************************************************************************/
		void SetEditorEnabled(bool enabled);
		
		/************************************************************************/
		/*!
		\brief
			Pauses or unpauses the simulation.

		\param[in] paused
			- Switch to pause or unpause the simulation.
		*/
		/************************************************************************/
		void SetSimulationPaused(bool paused);

		/************************************************************************/
		/*!
		\brief
			Sets ImGui theme style.
		*/
		/************************************************************************/
		void SetImThemeStyle();

		void PlayGame();
		void StopGame();
		void DisableFPSDisplay();
	};

	// Creates imgui context and initializes OpenGL for imgui panel
	inline ImguiSystem::ImguiSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
		// Check for UI toggle input event
		m_dispatcher->Subscribe<InputEvent>([this](const InputEvent& event)
			{
				auto factory = m_coord.GetSystem<EntityFactory>();

				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::ToggleUIEditor:
						// when Editor currently on -> turn off
						if (m_toggleEditor) { PlayGame(); }

						SetEditorEnabled(!m_toggleEditor);

						// when Editor off -> on
						if (m_toggleEditor) { StopGame(); }
						break;

					case InputEvent::InputAction::UndoStep:
						
						if (factory)
						{
							NightSnack::UndoSystem::Instance().PerformUndo(m_coord, *factory);
						}
						break;
					case InputEvent::InputAction::Copy:
						if (event.pressed == InputEvent::InputActionState::Pressed) {
							Vec2 p = m_coord.GetComponent<Transform>(m_scenePanel->GetEditorCamera()).pos;
							factory->Clone(*m_selectedEntity, p);
						}
						break;
					}
				}
			});

		Init();
	}

	// Destroys imgui context and shuts down OpenGL and GLFW for imgui
	inline ImguiSystem::~ImguiSystem() {
		ImguiSystem::ResetAll();
	}

	// Initializes ImGui context and configures settings
	inline void ImguiSystem::Init() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Configure ImGui IO settings
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |=
			ImGuiConfigFlags_DockingEnable |
			ImGuiConfigFlags_ViewportsEnable |
			ImGuiConfigFlags_DpiEnableScaleFonts;

		// Set ImGui style
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			SetImThemeStyle();
		}

		ImGui_ImplGlfw_InitForOpenGL(m_coord.GetSystem<ApplicationSystem>()->GetNativeHandle(), true);
		ImGui_ImplOpenGL3_Init("#version 450");

		// Initialize shared data and panels
		m_selectedEntity = std::make_shared<Entity>(INVALID_ENTITY);
		m_uiNames		 = std::make_shared<std::unordered_map<Entity, std::string>>();
		m_hierarchyPanel = std::make_unique<HierarchyPanel>(m_coord, m_selectedEntity, m_uiNames);
		m_inspectorPanel = std::make_unique<InspectorPanel>(m_coord, m_selectedEntity, m_uiNames, m_tilemap);
		m_scenePanel	 = std::make_unique<ScenePanel>(m_coord, m_selectedEntity, m_uiNames, m_tilemap);
		m_optionsPanel	 = std::make_unique<OptionsPanel>(m_coord);
		m_assetsPanel	 = std::make_unique<AssetsPanel>(m_coord);
		m_animationPanel = std::make_unique<AnimationPanel>(m_coord, m_selectedEntity);
	}	

	// Sets up and draws UI panels
	inline void ImguiSystem::Update() {

		if (!m_toggleEditor) { return; }
		if (!m_isPlaying) { m_coord.Pause(); }

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// DockSpace setup
		SetupDockspace();

		// Panels Drawing
		if (m_showHierarchy && m_hierarchyPanel) m_hierarchyPanel->Draw();
		if (m_showInspector && m_inspectorPanel) m_inspectorPanel->Draw();
		if (m_showScene && m_scenePanel)		 m_scenePanel->Draw();
		if (m_showOptions && m_optionsPanel)	 m_optionsPanel->Draw();
		if (m_showAssets && m_assetsPanel)		 m_assetsPanel->Draw();
		if (m_showAnimation && m_animationPanel) m_animationPanel->Draw();

		// render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render Panels outside of window
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup);
		}
	}

	// Destroys and resets Imgui & all panels
	inline void ImguiSystem::ResetAll() {
		if (ImGui::GetCurrentContext())
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		m_hierarchyPanel.reset();
		m_inspectorPanel.reset();
		m_scenePanel.reset();
		m_optionsPanel.reset();
		m_assetsPanel.reset();
		m_animationPanel.reset();
		m_selectedEntity.reset();
		m_uiNames.reset();

		// Clears Undo stack upon reset
		NightSnack::UndoSystem::Instance().Clear();
	}

	// Resets the game to initial state
	inline void ImguiSystem::ResetGame() {
		//m_coord.GetSystem<Scene::SceneManager>()->CacheLevel();
	}

	// Sets up the main dockspace for the editor panels
	inline void ImguiSystem::SetupDockspace() {
		// Padding removal for dockspace window
		ImGui::GetStyle().WindowPadding = ImVec2(0, 0);
		ImGui::GetStyle().WindowBorderSize = 0.0f;

		// Dockspace window flags
		ImGuiWindowFlags window_flags =
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		// Set next window to cover entire viewport
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		static ImVec2 lastViewportSize = ImVec2(0, 0);
		bool viewportResized = (lastViewportSize.x != viewport->Size.x || lastViewportSize.y != viewport->Size.y);

		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockspace_id);
		if (!node || node->IsEmpty() || viewportResized) {
			lastViewportSize = viewport->Size;
			ImGui::DockBuilderRemoveNode(dockspace_id); // reset dockspace layout
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			if (viewport->Size.x <= 0.f || viewport->Size.y <= 0.f) { return; }
			
			// Split the dockspace into regions
			ImGuiID dock_id_left, dock_id_right, dock_id_top, dock_id_bottom, dock_id_center;
			ImGuiID dock_id_left_top, dock_id_left_bottom, dock_id_center_right, dock_id_bottom_right, dock_id_right_top, dock_id_right_bottom;

			ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, &dock_id_left, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Right, 0.25f, &dock_id_right, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Up, 0.25f, &dock_id_top, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Down, 1.0f / 3.0f, &dock_id_bottom, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 1.0f / 3.0f, &dock_id_left_bottom, &dock_id_left_top);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Right, 0.3f, &dock_id_center_right, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_bottom, ImGuiDir_Right, 0.5f, &dock_id_bottom_right, &dock_id_bottom);
			ImGui::DockBuilderSplitNode(dock_id_center_right, ImGuiDir_Down, 0.5f, &dock_id_right_bottom, &dock_id_right_top);


			// Dock panels to regions
			ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left_top);
			ImGui::DockBuilderDockWindow("Options", dock_id_left_bottom);
			ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
			ImGui::DockBuilderDockWindow("Scene", dock_id_center);
			ImGui::DockBuilderDockWindow("Assets", dock_id_bottom);
			ImGui::DockBuilderDockWindow("Animation", dock_id_bottom);
			ImGui::DockBuilderDockWindow("Asset Properties", dock_id_right_top);
			ImGui::DockBuilderDockWindow("Tile Editor", dock_id_right_bottom);
			ImGui::DockBuilderDockWindow("Properties:", dock_id_bottom_right);

			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::Begin("DockSpace", nullptr, window_flags);
		DrawMenuBar();
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
		ImGui::End();
	}

	// Draws the main menu bar for the editor
	inline void ImguiSystem::DrawMenuBar() {
		if (ImGui::BeginMenuBar())
		{
			static bool showLevelLoadErrorPopup = false;

			if (ImGui::BeginMenu("File"))
			{
				auto factory = m_coord.GetSystem<EntityFactory>();

				// Save current level
				static char levelName[128] = "NewLevel";
				ImGui::InputText("Level Name", levelName, IM_ARRAYSIZE(levelName));
				ImGui::SameLine();
				static bool overwrite = true;
				ImGui::Checkbox("Overwrite", &overwrite);
				ImGui::SameLine();
				if (ImGui::Button("Save")) {
					auto scene = m_coord.GetSystem<Scene::SceneManager>();
					if (scene)
					{
						DisableFPSDisplay();

						// Saves level and resets editor camera
						m_scenePanel->DestroyEditorCamera();
						scene->SaveLevel(levelName, overwrite);
						m_scenePanel->CreateEditorCamera();
					}
				}

				// Load level by name
				static char loadLevelName[128] = "Level1";
				ImGui::InputText("Load Level", loadLevelName, IM_ARRAYSIZE(loadLevelName));
				ImGui::SameLine();
				if (ImGui::Button("Load")) {
					auto scene = m_coord.GetSystem<Scene::SceneManager>();
					if (scene)
					{
						const auto& availableLevels = scene->GetAvailableLevels();
						std::string requestedLevel = std::string(loadLevelName);
						if (availableLevels.find(requestedLevel) != availableLevels.end())
						{
							DisableFPSDisplay();

							// Loads level and resets editor camera
							m_scenePanel->DestroyEditorCamera();
							scene->LoadLevel(loadLevelName);
							m_scenePanel->CreateEditorCamera();

							// Clears Undo stack
							NightSnack::UndoSystem::Instance().Clear();
						}
						else
						{
							// Level does not exist, show popup
							showLevelLoadErrorPopup = true;
							ImGui::OpenPopup("Level Load Error");
						}
					}
				}
				if (showLevelLoadErrorPopup)
				{
					ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
				}
				// ImGui popup for level load error
				if (ImGui::BeginPopupModal("Level Load Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					float popupWidth = ImGui::GetWindowSize().x;
					const char* msg = "Level '%s' does not exist!";
					char buffer[256];
					snprintf(buffer, sizeof(buffer), msg, loadLevelName);

					ImVec2 textSize = ImGui::CalcTextSize(buffer);
					ImGui::SetCursorPosX((popupWidth - textSize.x) * 0.5f);
					ImGui::Text("%s", buffer);

					ImGui::Separator();

					// PopUp OK button
					float buttonWidth = 80.0f;
					float buttonX = (popupWidth - buttonWidth) * 0.5f;
					ImGui::SetCursorPosX(buttonX);

					if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
					{
						showLevelLoadErrorPopup = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Open Default Level")) {
					auto scene = m_coord.GetSystem<Scene::SceneManager>();
					if (scene)
					{
						DisableFPSDisplay();

						// Loads Default level
						m_scenePanel->DestroyEditorCamera();
						scene->LoadLevel("DefaultLevel");
						m_scenePanel->CreateEditorCamera();

						// Clears Undo stack
						NightSnack::UndoSystem::Instance().Clear();
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Hierarchy", nullptr, &m_showHierarchy);
				ImGui::MenuItem("Options", nullptr, &m_showOptions);
				ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
				ImGui::MenuItem("Scene", nullptr, &m_showScene);
				ImGui::MenuItem("Assets", nullptr, &m_showAssets);
				ImGui::MenuItem("Animation", nullptr, &m_showAnimation);

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Game"))
			{
				// Play
				if (!m_isPlaying)
				{
					if (ImGui::MenuItem("Play"))
					{
						DisableFPSDisplay();
						//SetSimulationPaused(false);
						*m_selectedEntity = INVALID_ENTITY;
						m_scenePanel->DestroyEditorCamera();
						m_coord.GetSystem<Scene::SceneManager>()->CacheLevel();
						m_scenePanel->CreateEditorCamera();

						// Only Cache if not already playing
						m_isPlaying = true;

						m_coord.Unpause();
					}
				}
				else
				{
					// Disable Play button when already playing
					ImGui::BeginDisabled();
					ImGui::MenuItem("Play");
					ImGui::EndDisabled();
				}

				// Pause
				if (m_isPlaying)
				{
					auto physics = m_coord.GetSystem<PhysicsSystem>();
					//bool isPaused = physics && physics->IsDebugMode();
					if (ImGui::MenuItem("Pause"))
					{
						//m_isPlaying = false;
						//SetSimulationPaused(true);
						m_coord.Pause();
					}
				}
				else
				{
					// Disable Pause button when already paused
					ImGui::BeginDisabled();
					ImGui::MenuItem("Pause");
					ImGui::EndDisabled();
				}

				// Stop
				if (ImGui::MenuItem("Stop"))
				{
					DisableFPSDisplay();

					// Restore level to cached state
					m_coord.Pause();
					m_isPlaying = false;

					//SetSimulationPaused(true);
					*m_selectedEntity = INVALID_ENTITY;
					m_scenePanel->DestroyEditorCamera();
					m_coord.GetSystem<Scene::SceneManager>()->RestoreLevel();
					m_scenePanel->CreateEditorCamera();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	// Enables or disables the editor UI
	inline void ImguiSystem::SetEditorEnabled(bool enabled) {
		if (enabled && !ImGui::GetCurrentContext()) {
			Init();

			*m_selectedEntity = INVALID_ENTITY;
			m_scenePanel->ResetTexture();
		}
		else if (!enabled && ImGui::GetCurrentContext()) {

			// Properly shutdown backends before destroying context
			ImguiSystem::ResetAll();

			// Update application viewport on editor close
			auto appSystem = m_coord.GetSystem<ApplicationSystem>();
			if (appSystem)
			{
				int width = 0, height = 0;
				glfwGetFramebufferSize(appSystem->GetNativeHandle(), &width, &height);
				appSystem->OnResize(width, height);
			}
		}
		m_toggleEditor = enabled;
	}

	// Pauses or unpauses the simulation
	inline void ImguiSystem::SetSimulationPaused(bool paused) {
		if (m_dispatcher) {
			auto physics = m_coord.GetSystem<PhysicsSystem>();
			if (physics && physics->IsDebugMode() != paused) {
				m_dispatcher->QueueEvent(std::make_unique<InputEvent>
					(InputEvent::InputAction::PhysicsDebugMode, InputEvent::InputActionState::Pressed));
			}
		}
	}

	inline void ImguiSystem::PlayGame() {

		DisableFPSDisplay();

		// Only Cache if not already playing
		if (!m_isPlaying) {
			// Cache current level state
			m_scenePanel->DestroyEditorCamera();
			m_coord.GetSystem<Scene::SceneManager>()->CacheLevel();
			m_scenePanel->CreateEditorCamera();
		}

		m_coord.Unpause();
		SetSimulationPaused(false);
		m_isPlaying = true;
		SetSimulationPaused(false);
	}
	inline void ImguiSystem::StopGame() {
		DisableFPSDisplay();

		// Restore level to cached state
		m_coord.Pause();
		//SetSimulationPaused(true);
		m_isPlaying = false;

		m_scenePanel->DestroyEditorCamera();
		m_coord.GetSystem<Scene::SceneManager>()->RestoreLevel();
		m_scenePanel->CreateEditorCamera();

	}

	inline void ImguiSystem::DisableFPSDisplay() {
		auto app = m_coord.GetSystem<ApplicationSystem>();
		if (app)
		{
			app->m_showFPS = false;
			app->ToggleFPSDisplay();
		}
	}

	// Sets ImGui theme style
	inline void ImguiSystem::SetImThemeStyle() {
		// Comfy Purple style from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.1f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 10.0f;
		style.WindowBorderSize = 0.0f;
		style.WindowMinSize = ImVec2(30.0f, 30.0f);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Right;
		style.ChildRounding = 5.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 10.0f;
		style.PopupBorderSize = 0.0f;
		style.FramePadding = ImVec2(5.0f, 3.5f);
		style.FrameRounding = 5.0f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(5.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 5.0f;
		style.ColumnsMinSpacing = 5.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 15.0f;
		style.GrabRounding = 5.0f;
		style.TabRounding = 5.0f;
		style.TabBorderSize = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.360515f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
		style.Colors[ImGuiCol_DockingEmptyBg] = style.Colors[ImGuiCol_WindowBg];
		style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38039216f, 0.42352942f, 0.57254905f, 0.54901963f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.25882354f, 0.25882354f, 0.25882354f, 0.0f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 0.0f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15686275f, 0.15686275f, 0.15686275f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.23529412f, 0.23529412f, 0.23529412f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.4509804f, 1.0f, 0.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13333334f, 0.25882354f, 0.42352942f, 0.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.29411766f, 0.29411766f, 0.29411766f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.7372549f, 0.69411767f, 0.8862745f, 0.54901963f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.2901961f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03433478f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.5019608f, 0.3019608f, 1.0f, 0.54901963f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
	}

}	// NightSnack
