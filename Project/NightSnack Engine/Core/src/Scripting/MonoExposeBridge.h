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
	//Types of enums supported
	enum class MonoValueType
	{
		MONO_INT,
		MONO_BOOL,
		MONO_FLOAT,
		MONO_STRING,
		MONO_VECTOR2,
		MONO_VECTOR3,
		MONO_UNSUPPORTED
	};

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
	};

	//This class reasds 
	class MonoExposeBridge {

	public:
		MonoExposeBridge() {};

		void LoadScriptExposedFields(void* scriptInstance);

		const std::vector<ExposedField> GetExposedFields(void* scriptInstance);

		ExposedValue ReadValue(void* scriptInstance, const ExposedField& field);
		void WriteValue(void* scriptInstance, const ExposedField& field, const ExposedValue& value);
		
		void UnloadAll();

		void PrintAllExposedFields();

		static Vec2 ConvertVec2(const ExposedValue& value);
		static Vec3 ConvertVec3(const ExposedValue& value);

	private:
		bool FieldHasExposeAttribute(MonoClass* monoClass, MonoClassField* field);
		MonoValueType MapMonoType(MonoClass* clas, size_t& outSize);

		//Map of vector of exposed fields for each script
		std::map<void*, std::vector<ExposedField>> m_sciptsToFieldsMap;
	};
}