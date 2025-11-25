/******************************************************************************
 * File:        MonoInternalCalls.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan
 *
 * Description:
 * This file declares a CORE_API function that will link the C# Engine API in
 * ManagedScripts to NightSnack's ScriptInterface.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include "ImportExport.h"

namespace NightSnack::Scripting {

	// Forward declaration
	class ScriptInterface;

	void RegisterMonoInternalCalls(ScriptInterface* scriptI);

}	// NightSnack::Scripting
