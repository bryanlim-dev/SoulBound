#include "Graphics/Renderer.h"
#include "AppWindow/Window.h"

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
	Mesh point = Mesh::CreatePoint();
	Mesh line = Mesh::CreateLine();
	Mesh rectangle = Mesh::CreateSquare();
	Mesh circle = Mesh::CreateCircle();

	// Create the different materials
	Material red(&shader, glm::vec3(1, 0, 0));
	Material blue(&shader, glm::vec3(0, 0, 1));
	Material green(&shader, glm::vec3(0, 1, 0));

	// Create the renderables with customisable mesh and material
	Renderable redPoint{ &point, &red };
	Renderable greenLine{ &line, &green };
	Renderable redQuad{ &rectangle, &red };
	Renderable greenCircle{ &circle, &green };

	// Add the renderables to the pool of objects
	//renderer.AddRenderable(greenCircle);
	//renderer.AddRenderable(blueQuad);
	//renderer.AddRenderable(redPoint);
	//renderer.AddRenderable(greenLine);

	Transform transform{
		glm::vec2(200.f,-300.f),
		0.f,
		glm::vec2(500.f, 500.f)
	};
	GameObject obj1(transform, redQuad);
	renderer.AddGameObject(&obj1);

	Transform transformCircle{
		glm::vec2(0.f, 0.f),
		0.f,
		glm::vec2(300.f, 300.f)
	};
	GameObject obj2(transformCircle, greenCircle);
	renderer.AddGameObject(&obj2);




#pragma endregion

	// Game loop;
	while (!window.ShouldClose()) {
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.DrawAll();

		// CoreEngine.Update();

		window.SwapBuffers();
		window.PollEvents();
	}

	// Clean termination;
  return 0;
}