#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../GameObject.h"
#include "Graphics/Camera.h"

class Renderer {

public:

	// Add a renderable object when spawning entities
	//void AddRenderable(const Renderable& renderable);
	void AddGameObject(GameObject* obj);

	// Remove all objects 
	void Clear();

	// Main render loop
	void DrawAll();
	
private:

	//std::vector<Renderable> objects;
	std::vector<GameObject*> objects;
	Camera camera;
};