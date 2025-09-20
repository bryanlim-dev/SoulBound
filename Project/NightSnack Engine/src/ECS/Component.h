#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

struct IComponent{};

struct Archetype {
	// e.g: <"Position", static_pointer_cast<IComponent>(Position{ 1.f, 2.5f })>; components["Position"] = 1.f, 2.5f;
	std::unordered_map<std::string, IComponent> components;
};

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
    bool colliding{ false };
};

// Add more Components here if necessary...

#endif // COMPONENT_HPP
