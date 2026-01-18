/******************************************************************************
 * File:        MonoExposeBridge.cpp
 * Project:     NightSnack
 * Author(s):	Clement Ang Yang Ying (Primary, 100%) - Created class and logic
 *
 * Description:
 * This file contains the deceleration of the class MonoExposeBridge. This class
 * helps expose C# script fields marked with [SeralizeField] attribute to the engine.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "MonoExposeBridge.h"

#include "mono/metadata/reflection.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/metadata.h"

#include <iostream>

namespace NightSnack::Scripting {
	void MonoExposeBridge::LoadScriptExposedFields(void* scriptInstance)
	{
		if (!scriptInstance)
			return;

		//Erase if alr loaded
		if (m_sciptsToFieldsMap.find(scriptInstance) != m_sciptsToFieldsMap.end())
			m_sciptsToFieldsMap.erase(scriptInstance);


		MonoObject* monoObj = reinterpret_cast<MonoObject*>(scriptInstance); //Get mono object from void*
		MonoClass* monoClass = mono_object_get_class(monoObj); //Get mono class

		std::vector<ExposedField> fieldVec; //vector of exposed fields

		//Temp values to loop through
		void* it = nullptr;
		MonoClassField* monoField = nullptr;

		//Loop through all exposed fields
		while ((monoField = mono_class_get_fields(monoClass, &it)) != nullptr)
		{
			//Check for [SerializeField]
			if (!FieldHasExposeAttribute(monoClass, monoField))
				continue; // None, ignore this one

			uint32_t flags = mono_field_get_flags(monoField);
			//Skip static   
			if (flags & MONO_FIELD_ATTR_STATIC)
				continue;

			//Skip const fields  
			if (flags & MONO_FIELD_ATTR_LITERAL)
				continue;

			//Has has a exposed value
			//Get variables and put them into struct
			ExposedField field;
			field.m_monoField = monoField;

			const char* fieldName = mono_field_get_name(monoField);
			field.m_name = fieldName ? fieldName : "";

			//Get type of field
			MonoType* monoType = mono_field_get_type(monoField);
			MonoClass* fieldClass = mono_class_from_mono_type(monoType);
			const char* typeName = mono_class_get_name(fieldClass);
			field.m_typeName = typeName;
			field.m_scriptInstance = monoObj;

			// Map type to enum + struct size
			field.m_valueType = MapMonoType(fieldClass, field.m_sizeBytes);

			fieldVec.push_back(field);
		}

		//Shove the data into a map
		m_sciptsToFieldsMap.insert({ scriptInstance, fieldVec });
	}
	std::vector<ExposedField> MonoExposeBridge::GetExposedFields(void* scriptInstance)
	{
		//LoadScriptExposedFields(scriptInstance);
		static std::vector<ExposedField> empty;

		//Find the vector belonging to the script
		auto it = m_sciptsToFieldsMap.find(scriptInstance);
		if (it == m_sciptsToFieldsMap.end())
			return empty; //Not found return empty vec

		return it->second;
	}

	ExposedValue MonoExposeBridge::ReadValue(void* scriptInstance, const ExposedField& field)
	{
		if (!scriptInstance)
			return ExposedValue{};

		ExposedValue result;
		MonoObject* monoObj = reinterpret_cast<MonoObject*>(scriptInstance);

		switch (field.m_valueType)
		{
		case MonoValueType::MONO_INT:
		{
			int32_t tmp = 0;
			mono_field_get_value(monoObj, field.m_monoField, &tmp);
			result.m_i = tmp;
			break;
		}
		case MonoValueType::MONO_FLOAT:
		{
			float tmp = 0.0f;
			mono_field_get_value(monoObj, field.m_monoField, &tmp);
			result.m_f = tmp;
			break;
		}
		case MonoValueType::MONO_BOOL:
		{
			bool tmp = false;
			mono_field_get_value(monoObj, field.m_monoField, &tmp);
			result.m_b = tmp;
			break;
		}
		case MonoValueType::MONO_STRING:
		{
			MonoString* str = nullptr;
			mono_field_get_value(monoObj, field.m_monoField, &str);
			if (str)
			{
				char* raw = mono_string_to_utf8(str);
				result.m_s = raw;
				mono_free(raw);
			}
			break;
		}
		case MonoValueType::MONO_VECTOR2:
		case MonoValueType::MONO_VECTOR3:
		{
			if (field.m_sizeBytes > 0)
			{
				std::vector<uint8_t> temp(field.m_sizeBytes);
				mono_field_get_value(monoObj, field.m_monoField, temp.data());
				result.m_structs = std::move(temp);
			}
			break;
		}

		case MonoValueType::MONO_SCRIPT:
		{
			MonoObject* obj = nullptr;
			mono_field_get_value(monoObj, field.m_monoField, &obj);

			result.m_object = obj;
			result.m_s = GetScriptName(obj); // For inspector display

			break;
		}
		case MonoValueType::MONO_UNSUPPORTED:
		default:
		{
			// Handle reference / user-defined types explicitly:
			// e.g. read a MonoObject* and store a placeholder or pointer string
			MonoObject* obj = nullptr;
			mono_field_get_value(monoObj, field.m_monoField, &obj);
			if (obj)
			{
				// store some representation (e.g., class name or pointer) or skip
				result.m_s = mono_class_get_name(mono_object_get_class(obj));
			}
			break;
		}
		}

		return result;
	}

	void MonoExposeBridge::WriteValue(void* scriptInstance, const ExposedField& field, const ExposedValue& value)
	{
		MonoObject* monoObj = reinterpret_cast<MonoObject*>(scriptInstance);

		//Get the type of value
		switch (field.m_valueType)
		{
		case MonoValueType::MONO_INT:
			mono_field_set_value(monoObj, field.m_monoField, (void*)&value.m_i);
			return;

		case MonoValueType::MONO_FLOAT:
			mono_field_set_value(monoObj, field.m_monoField, (void*)&value.m_f);
			return;

		case MonoValueType::MONO_BOOL:
			mono_field_set_value(monoObj, field.m_monoField, (void*)&value.m_b);
			return;

		case MonoValueType::MONO_STRING:
		{
			MonoString* str = mono_string_new(mono_domain_get(), value.m_s.c_str());
			mono_field_set_value(monoObj, field.m_monoField, (void*)str);
		}
		return;

		default:
			mono_field_set_value(monoObj, field.m_monoField, (void*)value.m_structs.data());
			return;
		}
	}

	void MonoExposeBridge::PrintAllExposedFields()
	{
		int scriptIdx = 0;
		for (auto& pair : m_sciptsToFieldsMap)
		{
			void* scriptInstance = pair.first;
			const std::vector<ExposedField>& fields = pair.second;

			std::cout << "Script Instance" << scriptInstance << ", fields: " << fields.size() << std::endl;

			for (auto& field : fields)
			{
				std::cout << "Field: " << field.m_name << std::endl;
				std::cout << "Typename: " << field.m_typeName << std::endl;
				std::cout << "ValueType: " << (int)field.m_valueType << std::endl;
				std::cout << "Byte size: " << field.m_sizeBytes << std::endl;
				std::cout << "MonoField: " << field.m_monoField << std::endl;
			}
				std::cout << std::endl;

			scriptIdx++;
		}
	}

	Vec2 MonoExposeBridge::ConvertVec2(const ExposedValue& value)
	{
		Vec2 result{};

		//Make sure struct size is correct
		if (value.m_structs.size() != sizeof(Vec2))
			return result;

		memcpy(&result, value.m_structs.data(), sizeof(Vec2));

		return result;
	}

	Vec3 MonoExposeBridge::ConvertVec3(const ExposedValue& value)
	{
		Vec3 result{};

		//Make sure struct size is correct
		if (value.m_structs.size() != sizeof(Vec3))
			return result;

		memcpy(&result, value.m_structs.data(), sizeof(Vec3));

		return result;
	}

	void MonoExposeBridge::ConvertValueFromVec2(ExposedValue& value, Vec2 vec2)
	{
		//Make sure struct size is correct
		if (value.m_structs.size() != sizeof(Vec2))
			return;

		memcpy(value.m_structs.data(), &vec2, sizeof(Vec2));
	}

	void MonoExposeBridge::ConvertValueFromVec3(ExposedValue& value, Vec3 vec3)
	{
		//Make sure struct size is correct
		if (value.m_structs.size() != sizeof(Vec3))
			return;

		memcpy(value.m_structs.data(), &vec3, sizeof(Vec3));
	}

	bool MonoExposeBridge::IsScriptClass(MonoClass* monoClass)
	{

		while (monoClass)
		{
			//Checks if the class type is script
			if (strcmp(mono_class_get_name(monoClass), "Script") == 0)
				return true;

			//Sets the parent as the mono object, so we can check name of parent in next loop
			//Returns null if no parent class
			monoClass = mono_class_get_parent(monoClass);
		}

		//Parent class isnt script, it is not script class
		return false;
	}

	std::string MonoExposeBridge::GetScriptName(MonoObject* obj)
	{
		if (obj != nullptr)
		{
			MonoClass* monoClass = mono_object_get_class(obj);

			if(monoClass)
				return mono_class_get_name(monoClass);
		}
		else
			return "ErrorNotScript";

	}

	void MonoExposeBridge::UnloadAll()
	{
		m_sciptsToFieldsMap.clear();
	}

	bool MonoExposeBridge::FieldHasExposeAttribute(MonoClass* monoClass, MonoClassField* field)
	{

		MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(monoClass, field);
		if (attrInfo == nullptr)
			return false;


		//Mono Image apprently stores data not pictures
		MonoImage* image = mono_class_get_image(monoClass);

		MonoClass* exposeAttrClass = mono_class_from_name(image, "ManagedScripts", "SerializeFieldAttribute");

		//Check if got attribute loaded
		if (exposeAttrClass == nullptr)
		{
			mono_custom_attrs_free(attrInfo);
			return false;
		}


		bool hasExpose = mono_custom_attrs_has_attr(attrInfo, exposeAttrClass);

		//Clean up
		mono_custom_attrs_free(attrInfo);

		return hasExpose;
	}


	MonoValueType MonoExposeBridge::MapMonoType(MonoClass* typeClass, size_t& outSize)
	{
		const char* className = mono_class_get_name(typeClass);
		const char* nameSpace = mono_class_get_namespace(typeClass);

		//Split between standard/system types and self implement ones
		if (strcmp(nameSpace, "System") == 0)
		{
			if (strcmp(className, "Int32") == 0)
			{
				outSize = sizeof(int32_t);
				return MonoValueType::MONO_INT;
			}
			if (strcmp(className, "Single") == 0) //This is float
			{
				outSize = sizeof(float);
				return MonoValueType::MONO_FLOAT;
			}
			if (strcmp(className, "Boolean") == 0)
			{
				outSize = sizeof(bool);
				return MonoValueType::MONO_BOOL;
			}
			if (strcmp(className, "String") == 0)
			{
				outSize = sizeof(void*);
				return MonoValueType::MONO_STRING;
			}
		}
		else if (strcmp(nameSpace, "ManagedScripts") == 0)
		{
			if (strcmp(className, "Vector2") == 0)
			{
				outSize = sizeof(float) * 2;
				return MonoValueType::MONO_VECTOR2;
			}
			if (strcmp(className, "Vector3") == 0)
			{
				outSize = sizeof(float) * 3;
				return MonoValueType::MONO_VECTOR3;
			}
			if (IsScriptClass(typeClass))
			{
				outSize = sizeof(void*);
				return MonoValueType::MONO_SCRIPT;
			}
		}


		//Checks for if its a script outside of the managedscripts namespace
		if (IsScriptClass(typeClass))
		{
			outSize = sizeof(void*);
			return MonoValueType::MONO_SCRIPT;
		}

		//None of the above, unsupported
		outSize = 0;
		return MonoValueType::MONO_UNSUPPORTED;
	}
}
