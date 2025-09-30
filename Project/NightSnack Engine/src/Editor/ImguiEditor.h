#pragma once

#include "./ECS/ISystem.h"

class ImGuiEditor : public ISystem {

public:
	void Update(float dt, Coordinator& coord);
	void Init();

	Signature GetSignature() const { return Signature{}; }
	std::string GetName() const override { return "ImGuiEditor"; }

private:


};
