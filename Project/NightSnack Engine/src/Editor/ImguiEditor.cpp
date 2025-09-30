#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ImguiEditor.h"

void ImGuiEditor::Init(GLFWwindow* window) {
	// Setup ImGui context;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_io = ImGui::GetIO();
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable keyboard controls;
	m_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking;
	m_io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Multi-Viewport (optional);

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);        // <-- your GLFWwindow*
	ImGui_ImplOpenGL3_Init("#version 450");            // GLSL version string

}

void ImGuiEditor::Update(float dt, Coordinator& coord) {// Start new ImGui frame

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// ------------------------------
	// ImGui UI code here

	ImGui::Begin("Hello, ImGui!");
	ImGui::Text("This is an ImGui window!");
	ImGui::End();


	// ------------------------------

	// Render ImGui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Optional: Multi-viewport support
	if (m_io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup);
	}
}