/******************************************************************************
 * File:        MonoExposeBridge.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created class and logic
 *
 * Description:
 * This file contains the definition of the class MonoExposeBridge. This class
 * helps expose C# script fields marked with [SeralizeField] attribute to the engine.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once
#include <string>
#include <map>
#include <vector>

#include "mono/metadata/class.h"
#include "mono/metadata/object.h"
#include "mono/metadata/attrdefs.h"
#include "../Core/ECS/Component.h"

namespace NightSnack::Scripting {

	//Struct on [SerializeField] variable info
	struct ExposedField
	{
		std::string m_name{}; //C# field name
		std::string m_typeName{}; //type name (e.g ManagedScripts::Vector2)
		MonoClassField* m_monoField = nullptr; //handle for get/set
		MonoValueType m_valueType{}; //Type of value
		size_t m_sizeBytes{}; //byte size for structs
		MonoObject* m_scriptInstance = nullptr;
	};

	struct ExposedValue {
		//Union of simple data types
		union {
			int32_t m_i = 0;
			float m_f;
			bool m_b;
		};
		std::string m_s{};  //used when type is a string
		std::vector<uint8_t> m_structs; //used for when the type is a struct

		MonoObject* m_object; //Used for scripts
	};

	//This class reasds 
	class MonoExposeBridge {

	public:
		/************************************************************************/
		/*!
		\brief
		Default Constructor for MonoExposeBridge
		*/
		/************************************************************************/
		MonoExposeBridge() {};

		/************************************************************************/
		/*!
		\brief
		Loads all exposed fields from a script instance
		
		\param scriptInstance
		void ptr to script instance
		*/
		/************************************************************************/
		void LoadScriptExposedFields(void* scriptInstance);

		/************************************************************************/
		/*!
		\brief
		Returns all exposed fields from a previously loaded script instance
		
		\param scriptInstance
		void ptr to script instance

		\return
		vector of all exposed fields
		*/
		/************************************************************************/
		std::vector<ExposedField> GetExposedFields(void* scriptInstance);

		/************************************************************************/
		/*!
		\brief
		Gets value of an exposed field from a script instance and field

		\param scriptInstance
		void ptr to script instance of the owner of the ExposedField struct

		\param scriptInstance
		ExposedField struct of info of the value to extract

		\return
		ExposedValue union/struct of info containting the value
		*/
		/************************************************************************/
		ExposedValue ReadValue(void* scriptInstance, const ExposedField& field);

		/************************************************************************/
		/*!
		\brief
		Writes value of an exposed field to a script instance and field

		\param scriptInstance
		void ptr to script instance of the owner of the ExposedField struct

		\param scriptInstance
		ExposedField struct of info of the value to write to

		\param value
		ExposedValue union/struct ref of info containting the value to change
		*/
		/************************************************************************/
		void WriteValue(void* scriptInstance, const ExposedField& field, const ExposedValue& value);

		/************************************************************************/
		/*!
		\brief
		Unloads all exposed fields stored
		*/
		/************************************************************************/
		void UnloadAll();

		/************************************************************************/
		/*!
		\brief
		Prints all exposed fields
		*/
		/************************************************************************/
		void PrintAllExposedFields();

		/************************************************************************/
		/*!
		\brief
		Converts info in ExposedValue to Vec2

		\param value
		ExposedValue ref of value to convert (Needs to be Vector2 from C#)

		\return
		Vec2 of value
		*/
		/************************************************************************/
		static Vec2 ConvertVec2(const ExposedValue& value);

		/************************************************************************/
		/*!
		\brief
		Converts info in ExposedValue to Vec3

		\param value
		ExposedValue ref of value to convert (Needs to be Vector3 from C#)

		\return
		Vec3 of value
		*/
		/************************************************************************/
		static Vec3 ConvertVec3(const ExposedValue& value);

		/************************************************************************/
		/*!
		\brief
		Converts info in Vec2 to ExposedValue

		\param value
		ExposedValue ref of value to convert (Needs to be Vector2 from C#)

		\param vec2
		Vec2 of the value to get data from
		*/
		/************************************************************************/
		static void ConvertValueFromVec2(ExposedValue& value, Vec2 vec2);

		/************************************************************************/
		/*!
		\brief
		Converts info in Vec3 to ExposedValue

		\param value
		ExposedValue ref of value to convert (Needs to be Vector3 from C#)

		\param vec3
		Vec3 of the value to get data from
		*/
		/************************************************************************/
		static void ConvertValueFromVec3(ExposedValue& value, Vec3 vec3);

		/************************************************************************/
		/*!
		\brief
		Checks if a given MonoClass is a script
		
		\param monoClass
		MonoClass to check from
		
		\return
		true if it is a class
		false if it is not a class
		*/
		/************************************************************************/
		bool IsScriptClass(MonoClass* monoClass);

		/************************************************************************/
		/*!
		\brief
		Gets the name of a script

		\param obj
		MonoObject to check from

		\return
		string of the name of the script
		"ErrorNotScript" if it is not a script
		*/
		/************************************************************************/
		std::string GetScriptName(MonoObject* obj);

		/************************************************************************/
		/*!
		\brief
		Checks if a give script has the exposed variable

		\param obj
		MonoObject to check from

		\return
		string of the name of the script
		"ErrorNotScript" if it is not a script
		*/
		/************************************************************************/
		bool GetVariableMatchingType(void* scriptInstance, std::string varName, MonoValueType vauleType);

		/************************************************************************/
		/*!
		\brief
		Gets the id of the owner of the scsript
		
		\param obj
		MonoObject to check from
		
		\return
		id of the object
		-1 if invaid id
		*/
		/************************************************************************/
		int GetScriptOwnerID(MonoObject* obj);

	private:
		/************************************************************************/
		/*!
		\brief
		Checks if a script has [SerializeField] on any variables

		\param monoClass
		MonoClass ptr of the script class

		\param field
		MonoClassField of fields within the class to checl

		\return
		true - has [SerializeField] variables
		false - no [SerializeField] variables
		*/
		/************************************************************************/
		bool FieldHasExposeAttribute(MonoClass* monoClass, MonoClassField* field);


		/************************************************************************/
		/*!
		\brief
		Maps MonoClassField to MonoValueType enum

		\param clas
		MonoClass ptr of the script class variable

		\param outSize
		byte size of the value

		\return
		MonoValueType enum of the variable type
		*/
		/************************************************************************/
		MonoValueType MapMonoType(MonoClass* clas, size_t& outSize);

		//Map of vector of exposed fields for each script
		std::map<void*, std::vector<ExposedField>> m_sciptsToFieldsMap;
	};
}