#pragma once

#include "Mesh.h"
#include "Material.h"

struct Renderable {
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	bool wireframe = true; // Default is true, for debugging purposes
};