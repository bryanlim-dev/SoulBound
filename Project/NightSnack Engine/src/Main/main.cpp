#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "AppWindow/Window.h"
#include <iostream>

#include "Core.h"
#include "Systems/AllSystems.h"		// Include your system header in this file;

int main()
{
  // Enable memory leak checking at program exit
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  // Create Core engine, passing window parameters;
  CoreEngine Core(1600, 900, "NightSnack Engine");

  // Will register all components;
  // If you create new components, please register them in this function defined in Core.cpp;
  Core.Initialize();

  // Add the systems you want to the Core engine, Must be in specific order taught in GIT lessons;
  // i.e: Input-> Movement -> Collision -> Graphics;
  Core.AddSystem<MovementSystem>();

  // Add more systems here...

  // Core Application Loop;
  while (!Core.ShouldClose()) {
  	Core.Update();
  }

  // Clean termination;
  return 0;
}