#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "AppWindow/Window.h"

class Camera {

public:

	//Camera(float width, float height);

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();

private:

	glm::vec2 position{ 200.f, -300.f };
	float zoom{ 1.f };
	float width{ 1600.f }; // default width
	float height{ 900.f }; // default height
	

};