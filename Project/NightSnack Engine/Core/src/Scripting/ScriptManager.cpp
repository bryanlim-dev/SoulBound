#pragma once

#include "ScriptManager.h"

namespace NightSnack {

	void ScriptManager::Update(int steps) {
		// Skip updating of scripts if in debug mode;
		if (debugMode && !stepRequest) {
			return;
		}

		for (int s{}; s < steps; ++s) {

		}

		if (debugMode) stepRequest = false; // reset step request
    }


}	// NightSnack