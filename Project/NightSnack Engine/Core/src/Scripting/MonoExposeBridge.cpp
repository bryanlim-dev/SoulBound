#include "MonoExposeBridge.h"

#include "mono/metadata/reflection.h"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

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

	void MonoExposeBridge::UnloadAllFields()
	{
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
