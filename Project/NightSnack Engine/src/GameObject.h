#pragma once

#include "Graphics/Renderable.h"
#include "Transform.h"

class GameObject {

public:
	Transform transform;
	Renderable renderable;

	GameObject(const Transform& t, const Renderable& r)
		: transform(t), renderable(r) {
	}
};