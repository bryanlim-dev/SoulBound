#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
#include "Material.h"

struct Renderable {
	//Transform transform;
	Mesh* mesh;
	Material* material;
	bool wireframe = true; // Default is true, for debugging purposes
};

class Renderer {

public:

	// Add a renderable object when spawning entities
	void AddRenderable(const Renderable& renderable);

	// Remove all objects (e.g. per-frame rebuild if ECS is driving it)
	void Clear();

	// Main render loop
	void DrawAll();
	
private:

	std::vector<Renderable> objects;

};