/******************************************************************************
 * File:        ResourceManager.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created manager class and logic
 *
 * Description:
 * This file contains the declarations/definitions for a helper functions in the namespace
	ReousceSystem to help load/unload and return various values for the system
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <memory>
#include <string>
#include <ResourceManager/ResourceManager.h>

using namespace NightSnack;

namespace ResourceSystemHelper
{
	static bool GetResourceSystemWindowData(std::shared_ptr<ResourceManager> rm, std::string configFilePath, int& winHeight, int& winWidth, std::string& windowTitle)
	{
		size_t configKey = 0;
		rm->LoadJson(configFilePath, configKey);

		if (rm->ResourceLoaded(NightSnack::RESOURCE_JSON, configKey))
		{
			windowTitle = (rm->GetJson(configKey)["WindowConfig"].componentMap["WindowTitle"]);
			winWidth = std::stoi(rm->GetJson(configKey)["WindowConfig"].componentMap["Width"]);
			winHeight = std::stoi(rm->GetJson(configKey)["WindowConfig"].componentMap["Height"]);
			rm->UnloadJson(configKey);
			return true;
		}
		else
		{
			windowTitle = "Load Failed";
			winWidth = 100;
			winHeight = 100;
			return false;
		}

	}
};