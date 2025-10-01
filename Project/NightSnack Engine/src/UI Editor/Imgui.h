#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "ECS/ISystem.h"
#include "Systems/Application.h"

class ImguiSystem : public ISystem {

public:
	ImguiSystem(Coordinator& coord);
	~ImguiSystem();

	void Update(float dt, Coordinator& coord) override;

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
