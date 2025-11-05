#pragma once

#include "MathLibrary/Math.h"

namespace NightSnack {

	struct Force {

		Vec2 direction;
		float magnitude;

		float rotation;

		float lifetime;
		bool isActive;

	};

	class ForceManager {

	};

}	// NightSnack
