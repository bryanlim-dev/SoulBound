#pragma once

#include "Test.h"
#include <iostream>

namespace Core 
{
	void Application::Run()
	{
		int count = 0;
		while (1)
		{
			if (count == 1000)
				break;

			std::cout << count++ << '\n';
		}
	}

	void Application::HelloWorld()
	{
		std::cout << "Hello Native World!\n";
	}
}
