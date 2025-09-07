#include <Window.h>
#include "Renderer.h"

int main()
{
	// Create window (width, height, title);
	Window window(1600, 900, "NightSnack Engine");
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

#pragma region Temporary test code for rendering basic debugging shapes
	// Load shader
	Shader shader("../shaders/vertex_shader.vert", "../shaders/fragment_shader.frag");
	Renderer renderer;

	// Create the different meshes
	// NOTE: Positions for the meshs are in NDC coordinates currently
	// It will be changed according to our window resolution soon ...
	Mesh point = Mesh::CreatePoint({ 0.f,0.f });
	Mesh line = Mesh::CreateLine({ 0.f, 0.5f}, { 0.5f, 0.f });
	Mesh rectangle = Mesh::CreateRectangle({ -0.25f, 0.f }, 0.5f, 0.3f);
	Mesh circle = Mesh::CreateCircle({ 0.f, 0.f }, 0.3f);

	// Create the different materials
	Material red(&shader, glm::vec3(1, 0, 0));
	Material blue(&shader, glm::vec3(0, 0, 1));
	Material green(&shader, glm::vec3(0, 1, 0));

	// Create the renderables with customisable mesh and material
	Renderable redPoint{ &point, &red };
	Renderable greenLine{ &line, &green };
	Renderable blueQuad{ &rectangle, &blue };
	Renderable greenCircle{ &circle, &green };

	// Add the renderables to the pool of objects
	renderer.AddRenderable(greenCircle);
	renderer.AddRenderable(blueQuad);
	renderer.AddRenderable(redPoint);
	renderer.AddRenderable(greenLine);
#pragma endregion

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.DrawAll();

		window.SwapBuffers();
		window.PollEvents();
	}

	// Clean termination;
  return 0;
}