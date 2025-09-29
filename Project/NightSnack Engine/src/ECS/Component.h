#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

#include <json.hpp>
using json = nlohmann::json;

// ================================================================================================
// COMPONENTS
// ================================================================================================
struct Position {
  float x, y, z;
};

struct Rotation {
	float degrees;
};

struct Scale {
	float x, y;
};

struct Transform {
	Position pos;
	Rotation rot;
	Scale scale;
};

struct Velocity {
	float dx, dy;
};

struct Acceleration {
	float ax, ay;
};

struct Force {
	float fx{ 0.0 };
	float fy{ 0.0 };
};

struct Mass {
	float val = 1.0;
};

struct Sprite {
	std::string textureName;
};

struct Color {
	float r, g, b, a;
};

struct PlayerController {
    float moveSpeed;
    float jumpForce;
    // maybe can add abilities etc...
};

// ================================================================================================
// GRAPHICS COMPONENTS
// ================================================================================================

enum class RenderLayer {
	BACKGROUND = 0,
	DEFAULT,
	DEBUG,
	UI
};

struct Renderable {
	std::string meshName{};
	std::string materialName{};
	RenderLayer renderLayer{ RenderLayer::DEFAULT };
	bool isWireframe{ false };
	bool wasModified{ true };
	bool isSelected{ false };

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
    Trigger,  // Detects overlaps only
	Ground
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
// Ensure to add a Json overload below for your component to allow for Archetype Loading...

// ================================================================================================
// COMPONENT JSON OVERLOADS
// ================================================================================================
// POSITION / ROTATION / SCALE / TRANSFORM ====================
inline void from_json(const json& j, Position& p) {
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
}

inline void from_json(const json& j, Rotation& r) {
    j.at("degrees").get_to(r.degrees);
}

inline void from_json(const json& j, Scale& s) {
    j.at("x").get_to(s.x);
    j.at("y").get_to(s.y);
}

inline void from_json(const json& j, Transform& t) {
    t.pos = j.at("Position").get<Position>();
    t.rot = j.at("Rotation").get<Rotation>();
    t.scale = j.at("Scale").get<Scale>();
}

// PHYSICS ====================================================
inline void from_json(const json& j, Velocity& v) {
    j.at("dx").get_to(v.dx);
    j.at("dy").get_to(v.dy);
}

inline void from_json(const json& j, Acceleration& a) {
    j.at("ax").get_to(a.ax);
    j.at("ay").get_to(a.ay);
}

inline void from_json(const json& j, Force& f) {
    j.value("fx", 0.0f); // optional default
    j.value("fy", 0.0f);
    j.at("fx").get_to(f.fx);
    j.at("fy").get_to(f.fy);
}

inline void from_json(const json& j, Mass& m) {
    j.at("val").get_to(m.val);
}

// GRAPHICS ===================================================
inline void from_json(const json& j, Sprite& s) {
    j.at("textureName").get_to(s.textureName);
}

inline void from_json(const json& j, Color& c) {
    j.at("r").get_to(c.r);
    j.at("g").get_to(c.g);
    j.at("b").get_to(c.b);
    j.at("a").get_to(c.a);
}

// Renderable is tricky since Mesh* and Material* are runtime resources.
// Might instead serialize names/ids and resolve them later; Store meshes in an unorderedmap with std::string keys;
inline void from_json(const json& j, Renderable& r) {

    j.at("meshName").get_to(r.meshName);
    j.at("materialName").get_to(r.materialName);

    if (j.contains("isWireframe"))
        j.at("isWireframe").get_to(r.isWireframe);
    else
        r.isWireframe = false;

    r.wasModified = true;

}

// Camera =====================================================
inline void from_json(const json& j, Camera& c) {
    j.at("isMainCam").get_to(c.isMainCam);
    j.at("zoom").get_to(c.zoom);
    j.at("width").get_to(c.width);
    j.at("height").get_to(c.height);
}

// COLLISION ==================================================
inline void from_json(const json& j, Collider& c) {
    std::string shapeStr, typeStr;
    j.at("shape").get_to(shapeStr);
    j.at("type").get_to(typeStr);

    if (shapeStr == "AABB") c.shape = ShapeType::AABB;
    else if (shapeStr == "Circle") c.shape = ShapeType::Circle;

    if (typeStr == "Solid") c.type = ColliderType::Solid;
    else if (typeStr == "Trigger") c.type = ColliderType::Trigger;

    j.at("isStatic").get_to(c.isStatic);

    if (c.shape == ShapeType::AABB) {
        j.at("width").get_to(c.width);
        j.at("height").get_to(c.height);
        c.radius = 0.f;
    }
    else if (c.shape == ShapeType::Circle) {
        j.at("radius").get_to(c.radius);
        c.width = 0.f;
        c.height = 0.f;
    }

    // Optional runtime-only flags (not usually in prefab JSON)
    c.isGrounded = j.value("isGrounded", false);
    c.colliding = j.value("colliding", false);
}

#endif // COMPONENT_HPP
