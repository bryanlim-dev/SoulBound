/******************************************************************************
 * File:        Imgui.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * UI system that will be the foundation of UI editor, makes use of Imgui library.
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
		Destroys imgui constext and shuts down OpenGL and GLFW for imgui.
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

		\param dt
		delta time
		\param coord
		Reference to ECS Coordinator.
		*/
		/************************************************************************/
		void Update(float dt, Coordinator& coord) override;

		/************************************************************************/
		/*!
		\brief
		Destroys and resets Imgui & all panels.
		*/
		/************************************************************************/
		void ResetAll();

		// Required by ISystem base class.
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "ImguiSystem"; }

	private:
		Coordinator& m_coord;
		Signature m_systemSignature{};
		EventDispatcher* m_dispatcher = nullptr;
		bool m_toggleEditor = false;

		std::shared_ptr<Entity> m_selectedEntity;
		std::unique_ptr<HierarchyPanel> m_hierarchyPanel;
		std::unique_ptr<InspectorPanel> m_inspectorPanel;
		std::unique_ptr<ScenePanel> m_scenePanel;
		std::unique_ptr<OptionsPanel> m_optionsPanel;
		std::unique_ptr<AssetsPanel> m_assetsPanel;

		void SetupDockspace();
		void DrawMenuBar();
		void SetEditorEnabled(bool enabled);
	};

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

	inline ImguiSystem::~ImguiSystem() {
		ImguiSystem::ResetAll();
	}

	inline void ImguiSystem::Init() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |=
			ImGuiConfigFlags_DockingEnable |
			ImGuiConfigFlags_ViewportsEnable |
			ImGuiConfigFlags_DpiEnableScaleFonts;

		ImGui::StyleColorsDark();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(m_coord.GetSystem<ApplicationSystem>()->GetNativeHandle(), true);
		ImGui_ImplOpenGL3_Init("#version 450");

		m_selectedEntity = std::make_shared<Entity>(INVALID_ENTITY);
		m_hierarchyPanel = std::make_unique<HierarchyPanel>(m_coord, m_selectedEntity);
		m_inspectorPanel = std::make_unique<InspectorPanel>(m_coord, m_selectedEntity);
		m_scenePanel	 = std::make_unique<ScenePanel>(m_coord);
		m_optionsPanel	 = std::make_unique<OptionsPanel>(m_coord);
		m_assetsPanel	 = std::make_unique<AssetsPanel>(m_coord);
	}

	inline void ImguiSystem::Update(float dt, Coordinator& coord) {
		(void)coord;
		(void)dt;

		if (!m_toggleEditor) { return; }

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// DockSpace setup
		SetupDockspace();
		// Panels Drawing
		if (m_hierarchyPanel) m_hierarchyPanel->Draw();
		if (m_inspectorPanel) m_inspectorPanel->Draw();
		if (m_scenePanel) m_scenePanel->Draw();
		if (m_optionsPanel) m_optionsPanel->Draw();
		if (m_assetsPanel) m_assetsPanel->Draw();


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
	}

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
			ImGuiID dock_id_left_top, dock_id_left_bottom;

			ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.20f, &dock_id_left, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Right, 0.20f, &dock_id_right, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Up, 0.25f, &dock_id_top, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_center, ImGuiDir_Down, 1.0f / 3.0f, &dock_id_bottom, &dock_id_center);
			ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 1.0f / 3.0f, &dock_id_left_bottom, &dock_id_left_top);

			// Dock panels to regions
			ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left_top);
			ImGui::DockBuilderDockWindow("Options", dock_id_left_bottom);
			ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
			ImGui::DockBuilderDockWindow("Scene", dock_id_center);
			ImGui::DockBuilderDockWindow("Assets", dock_id_bottom);

			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::Begin("DockSpace", nullptr, window_flags);
		DrawMenuBar();
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
		ImGui::End();
	}

	inline void ImguiSystem::DrawMenuBar() {
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {}
				if (ImGui::MenuItem("Open")) {}
				if (ImGui::MenuItem("Save")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Hierarchy")) {}
				if (ImGui::MenuItem("Inspector")) {}
				if (ImGui::MenuItem("Scene")) {}
				if (ImGui::MenuItem("Assets")) {}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Game"))
			{
				if (ImGui::MenuItem("Play")) {}
				if (ImGui::MenuItem("Pause")) {}
				if (ImGui::MenuItem("Stop")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}

	inline void ImguiSystem::SetEditorEnabled(bool enabled) {
		if (enabled && !ImGui::GetCurrentContext()) {
			Init();

			m_scenePanel->ResetTexture();
		}
		else if (!enabled && ImGui::GetCurrentContext()) {
			// Properly shutdown backends before destroying context
			ImguiSystem::ResetAll();
		}
		m_toggleEditor = enabled;
	}

}	// NightSnack
