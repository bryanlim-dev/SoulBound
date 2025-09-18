#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

struct IComponent{};

// ================================================================================================
// COMPONENTS
// ================================================================================================
struct Position : public IComponent {
  double x, y;
};

struct Rotation : public IComponent {
	float degrees;
};

struct Scale : public IComponent {
	double x, y;
};

struct Velocity : public IComponent {
  double dx, dy;
};

struct Sprite : public IComponent {
  std::string textureName;
};

struct Color : public IComponent {
	float r, g, b, a;
};

// ================================================================================================
// GRAPHICS COMPONENTS
// ================================================================================================

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
