#include "MonoExposeBridge.h"

#include "mono/metadata/reflection.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
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
			// Check for [Expose]
			if (!FieldHasExposeAttribute(monoClass, monoField))
				continue; // None, ignore this one

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

			// Map type to enum + struct size
			field.m_valueType = MapMonoType(fieldClass, field.m_sizeBytes);

			fieldVec.push_back(field);
		}

		//Shove the data into a map
		m_sciptsToFieldsMap.insert({ scriptInstance, fieldVec });
	}
	const std::vector<ExposedField>& MonoExposeBridge::GetExposedFields(void* scriptInstance)
	{
		//LoadScriptExposedFields(scriptInstance);

		//Find the vector belonging to the script
		auto it = m_sciptsToFieldsMap.find(scriptInstance);
		if (it == m_sciptsToFieldsMap.end())
			return std::vector<ExposedField>(); //Not found return empty vec

		return it->second;
	}

	ExposedValue MonoExposeBridge::ReadValue(void* scriptInstance, const ExposedField& field)
	{
		ExposedValue result;
		MonoObject* monoObj = reinterpret_cast<MonoObject*>(scriptInstance);

		//Temporary storage buffer
		std::vector<uint8_t> tempBuffer;
		tempBuffer.resize(field.m_sizeBytes);

		mono_field_get_value(monoObj, field.m_monoField, tempBuffer.data());

		switch (field.m_valueType)
		{
		case MonoValueType::MONO_INT:
			memcpy(&result.m_i, tempBuffer.data(), sizeof(int32_t));
			break;

		case MonoValueType::MONO_FLOAT:
			memcpy(&result.m_f, tempBuffer.data(), sizeof(float));
			break;

		case MonoValueType::MONO_BOOL:
			memcpy(&result.m_b, tempBuffer.data(), sizeof(bool));
			break;

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

		default:
			result.m_structs = tempBuffer;
			break;
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

		MonoClass* exposeAttrClass = mono_class_from_name(image, "ManagedScripts", "ExposeAttribute");

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
		}

		//None of the above, unsupported
		outSize = 0;
		return MonoValueType::MONO_UNSUPPORTED;
	}
}
