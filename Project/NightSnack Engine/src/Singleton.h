/******************************************************************************
 * File:        Singleton.h
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created template class
 *
 * Description:
 * This file contains the declarations for a template class Singleton
 * It is a class template for the Singleton Pattern where only 1 instance of a class may exist
 * It hides the default constructor and destructor
 * It deletes the copy and assigment constrcutors
 * 
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
#pragma once

namespace NightSnack {

	//Template class for a singleton
	template <typename T>
	class Singleton
	{
	public:
		static T* GetInstance()
		{
			static T* instance; //The only instance allowed

			if (!instance)
				instance = new T();

			return instance;
		}

	protected:

		//These are hidden so you cant access them
		Singleton() = default;
		virtual ~Singleton() = default;

	public:
		//Delete the copy constructor and assigment operator
		Singleton(Singleton const& rhs) = delete;
		Singleton& operator= (Singleton const& rhs) = delete;
	};

}	// NightSnack;