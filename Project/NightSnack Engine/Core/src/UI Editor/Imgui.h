/******************************************************************************
 * File:        Imgui.h
 * Project:     NightSnack
 * Author(s):	Wei Jie Lim (Primary, %)
 *				Chewn Thing Kwan (Secondary, %)
 *
 * Description: 
 * This file contains the definition of ImguiSystem which handles the 
 * ImGui user interface for the editor.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
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
		void Update(int steps) override;

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

		// Required by ISystem base class.
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "ImguiSystem"; }

	private:
		Coordinator& m_coord;
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;

		// Editor state
		bool m_toggleEditor = false;
		bool m_isPlaying = true;

		// Panels and shared data
		std::shared_ptr<std::unordered_map<Entity, std::string>> m_uiNames;
		std::shared_ptr<Entity> m_selectedEntity;
		std::unique_ptr<HierarchyPanel> m_hierarchyPanel;
		std::unique_ptr<InspectorPanel> m_inspectorPanel;
		std::unique_ptr<ScenePanel> m_scenePanel;
		std::unique_ptr<OptionsPanel> m_optionsPanel;
		std::unique_ptr<AssetsPanel> m_assetsPanel;

		// Panel visibility toggles
		bool m_showHierarchy = true;
		bool m_showInspector = true;
		bool m_showScene = true;
		bool m_showOptions = true;
		bool m_showAssets = true;

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
	};

	// Creates imgui context and initializes OpenGL for imgui panel
	inline ImguiSystem::ImguiSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
		// Check for UI toggle input event
		m_dispatcher->Subscribe<InputEvent>([this](const InputEvent& event)
			{
				if (event.pressed == InputEvent::InputActionState::Pressed) {
					switch (event.inputAction) {
					case InputEvent::InputAction::ToggleUIEditor:
						SetEditorEnabled(!m_toggleEditor);
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
		m_inspectorPanel = std::make_unique<InspectorPanel>(m_coord, m_selectedEntity, m_uiNames);
		m_scenePanel	 = std::make_unique<ScenePanel>(m_coord, m_selectedEntity);
		m_optionsPanel	 = std::make_unique<OptionsPanel>(m_coord);
		m_assetsPanel	 = std::make_unique<AssetsPanel>(m_coord);
	}

	// Sets up and draws UI panels
	inline void ImguiSystem::Update(int steps) {
		(void)steps;

		if (!m_toggleEditor) { return; }

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// DockSpace setup
		SetupDockspace();

		// Panels Drawing
		if (m_showHierarchy && m_hierarchyPanel) m_hierarchyPanel->Draw();
		if (m_showInspector && m_inspectorPanel) m_inspectorPanel->Draw();
		if (m_showScene && m_scenePanel) m_scenePanel->Draw();
		if (m_showOptions && m_optionsPanel) m_optionsPanel->Draw();
		if (m_showAssets && m_assetsPanel) m_assetsPanel->Draw();


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
		m_selectedEntity.reset();
		m_uiNames.reset();
	}

	// Resets the game to initial state
	inline void ImguiSystem::ResetGame() {
		auto factory = m_coord.GetSystem<EntityFactory>();
		if (factory)
		{
			factory->UnLoadLevel();
			factory->LoadLevel("../assets/GameData/DefaultLevel.json");
		}
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
			
			// Split the dockspace into 4 regions
			ImGuiID dock_id_left, dock_id_right, dock_id_top, dock_id_bottom, dock_id_center;
			ImGuiID dock_id_left_top, dock_id_left_bottom, dock_id_center_right;

			ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, &dock_id_left, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Right, 0.25f, &dock_id_right, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Up, 0.25f, &dock_id_top, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Down, 1.0f / 3.0f, &dock_id_bottom, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 1.0f / 3.0f, &dock_id_left_bottom, &dock_id_left_top);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Right, 0.3f, &dock_id_center_right, &dock_id_center);


			// Dock panels to regions
			ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left_top);
			ImGui::DockBuilderDockWindow("Options", dock_id_left_bottom);
			ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
			ImGui::DockBuilderDockWindow("Scene", dock_id_center);
			ImGui::DockBuilderDockWindow("Assets", dock_id_bottom);
			ImGui::DockBuilderDockWindow("Asset Properties", dock_id_center_right);

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
			// Implemented at later date
			if (ImGui::BeginMenu("File"))
			{
				//if (ImGui::MenuItem("New")) {}
				if (ImGui::MenuItem("Open")) {
					auto factory = m_coord.GetSystem<EntityFactory>();
					if (factory)
					{
						factory->LoadLevel("../assets/GameData/DefaultLevel.json");
					}
				}
				//if (ImGui::MenuItem("Save")) {}
				ImGui::Separator();
				//if (ImGui::MenuItem("Exit")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Hierarchy", nullptr, &m_showHierarchy);
				ImGui::MenuItem("Options", nullptr, &m_showOptions);
				ImGui::MenuItem("Inspector", nullptr, &m_showInspector);
				ImGui::MenuItem("Scene", nullptr, &m_showScene);
				ImGui::MenuItem("Assets", nullptr, &m_showAssets);

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Game"))
			{
				// Play
				if (!m_isPlaying) 
				{
					if (ImGui::MenuItem("Play"))
					{
						m_isPlaying = true;
						SetSimulationPaused(false);
					}
				}
				else
				{
					ImGui::BeginDisabled();
					ImGui::MenuItem("Play");
					ImGui::EndDisabled();
				}

				// Pause
				if (m_isPlaying)
				{
					auto physics = m_coord.GetSystem<PhysicsSystem>();
					bool isPaused = physics && physics->IsDebugMode();
					if (ImGui::MenuItem("Pause"))
					{
						m_isPlaying = false;
						SetSimulationPaused(true);
					}
				}
				else
				{
					ImGui::BeginDisabled();
					ImGui::MenuItem("Pause");
					ImGui::EndDisabled();
				}

				// Stop
				if (ImGui::MenuItem("Stop"))
				{
					m_isPlaying = false;
					SetSimulationPaused(true);
					ResetGame();
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

			m_scenePanel->ResetTexture();
		}
		else if (!enabled && ImGui::GetCurrentContext()) {
			// Destroys Editor Camera
			m_scenePanel->DestroyEditorCamera();

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

	// Sets ImGui theme style
	inline void ImguiSystem::SetImThemeStyle() {
		// Visual Studio style from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 0.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(4.0f, 3.0f);
		style.FrameRounding = 0.0f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.TabBorderSize = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921569f, 0.5921569f, 0.5921569f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_DockingEmptyBg] = style.Colors[ImGuiCol_WindowBg];
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.32156864f, 0.32156864f, 0.33333334f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529412f, 0.3529412f, 0.37254903f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529412f, 0.3529412f, 0.37254903f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.30588236f, 0.30588236f, 0.30588236f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2f, 0.2f, 0.21568628f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.32156864f, 0.32156864f, 0.33333334f, 1.0f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.11372549f, 0.5921569f, 0.9254902f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.22745098f, 0.22745098f, 0.24705882f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.46666667f, 0.78431374f, 1.0f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.14509805f, 0.14509805f, 0.14901961f, 1.0f);
	}

}	// NightSnack
