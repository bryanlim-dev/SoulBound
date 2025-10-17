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
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "ECS/ISystem.h"
#include "Systems/Application.h"

namespace NightSnack {

	class ImguiSystem : public ISystem {

	public:
		/************************************************************************/
		/*!
		\brief
		Creates imgui context and initializes OpenGL for imgui panel.
		*/
		/************************************************************************/
		ImguiSystem(Coordinator& coord);

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
		Called in Core update, sets up and draws UI panels.

		\param dt
		delta time
		\param coord
		Reference to ECS Coordinator.
		*/
		/************************************************************************/
		void Update(float dt, Coordinator& coord) override;

		// Required by ISystem base class.
		Signature GetSignature() const override { return m_systemSignature; }
		std::string GetName() const override { return "ImguiSystem"; }

	private:
		Coordinator& m_coord;

		Signature m_systemSignature{};

	};

	inline ImguiSystem::ImguiSystem(Coordinator& coord) : m_coord(coord) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(coord.GetSystem<ApplicationSystem>()->GetNativeHandle(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	inline ImguiSystem::~ImguiSystem() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	inline void ImguiSystem::Update(float dt, Coordinator& coord) {
		(void)coord;
		(void)dt;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
		ImGui::Begin("TestPanel");

		// stuff here...
		//ImGui::Text("Fortnite und trink Cola! YIPEEE!");

		ImGui::End();

		// render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup);
		}

	}

}	// NightSnack
