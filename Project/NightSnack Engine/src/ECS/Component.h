#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

// ================================================================================================
// COMPONENTS
// ================================================================================================

struct Position {
  double x, y;
};

struct Velocity {
  double dx, dy;
};

struct Sprite {
  std::string TextureName;
};

struct Color {
	float r, g, b, a;
};

// ================================================================================================
// GRAPHICS COMPONENTS
// ================================================================================================

struct Transform {
	glm::vec2 position{ 0.f,0.f };
	float rotation{ 0.f };
	glm::vec2 scale{ 1.f, 1.f };
};

struct Renderable {
	Mesh* mesh{ nullptr }; // Stores a pointer to a GPU mesh
	Material* material{ nullptr }; // Stores shader + color/texture information
	bool isWireframe{ true }; // default is wireframe mode
};

struct Camera {
	bool isMainCam{ true };
	float zoom{ 1.f };
	float width{ 1600.f };
	float height{ 900.f };
};

// Add more Components here if necessary...

#endif // COMPONENT_HPP
