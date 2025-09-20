#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

// ================================================================================================
// COMPONENTS
// ================================================================================================

struct Position {
  float x, y;
};

struct Rotation {
	float degrees;
};

struct Scale {
	float x, y;
};

struct Velocity {
  float dx, dy;
};

struct Acceleration {
	double ax, ay;
};

struct Force {
	double fx{ 0.0 };
	double fy{ 0.0 };
};

struct Mass {
	double val = 1.0;
};

struct Sprite {
  std::string textureName;
};

struct Color {
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

// COLLISION COMPONENTS;

enum class ShapeType {
    AABB,
    Circle
};

enum class ColliderType {
    Solid,   // Blocks movement
    Trigger  // Detects overlaps only
};

struct Collider {
	ShapeType shape;     // AABB or Circle
	ColliderType type;   // Solid or Trigger
	bool isStatic;       // true = immovable (walls), false = dynamic (player)

	// Shape data
	float width{ 1.0f };   // AABB width
	float height{ 1.0f };  // AABB height
	float radius{ 0.5f };  // Circle radius

	// Collision state
	bool isGrounded{ false };
	bool colliding{ false };
};

// Add more Components here if necessary...

#endif // COMPONENT_HPP
