/******************************************************************************
 * File:        main.cpp
 * Project:     NightSnack
 * Author(s):	Bryan Lim Jun Jie (Secondary, 5%) - Initialized Audio system and play background audio/ click sound using H key
 *				Clement Ang Yang Ying (Secondary, 5%) - Created/Inited Resource Manager, loaded & assigned window high/width/title into application
 *
 * Description:
 * This file contains the main function.
 * This file is the main entry point of the NightSnack Engine
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "Core/Core.h"

#include "Scripting/Testing/Test.h"
#include "Scripting/MonoMeta.h"

int main()
{
	// Enable memory leak checking at program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Create Core engine
	NightSnack::CoreEngine core;
	core.StartUp("../assets/Data/WindowConfig.json");
	core.Run();
	core.Terminate();

	return 0;

}