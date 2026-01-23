
#include "Serializer.h"
#include "Factory/EntityFactory.h"

#include <vector>

#if 1

namespace NightSnack {

	void to_json(json& j, Vec3 const& v) {
		j = { {"x", v.x}, {"y", v.y}, {"z", v.z} };
	}
	void from_json(json const& j, Vec3& v) {
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
		j.at("z").get_to(v.z);
	}

	void to_json(json& j, Vec2 const& v) {
		j = { {"x", v.x}, {"y", v.y} };
	}
	void from_json(json const& j, Vec2& v) {
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
	}

	// --------------------------------------------------------------
	// TRANSFORM
	// --------------------------------------------------------------
	void to_json(json& j, Transform const& t) {
		std::string layer;

		switch (t.layer) {
		case LayerType::_ENGINE: layer = "_Engine"; break;
		case LayerType::BACKGROUND: layer = "Background"; break;
		case LayerType::FOREGROUND: layer = "Foreground"; break;
		case LayerType::GAMEUI: layer = "GameUI"; break;
		case LayerType::_ENGINE2: layer = "_Engine2"; break;
		default: layer = "GameLayer"; break;
		}
		j = {
				{ "Position", { {"x", t.pos.x}, {"y", t.pos.y}, {"z", t.pos.z} } },
				{ "Rotation", t.rot },
				{ "Scale",    { {"x", t.scale.x}, {"y", t.scale.y} } },
				{ "UIBasePosition", { {"x", t.UIbasePos.x}, {"y", t.UIbasePos.y}, {"z", t.UIbasePos.z} } },
				{ "UIBaseScale",    { {"x", t.UIbaseScale.x}, {"y", t.UIbaseScale.y} } },
				{ "Layer", layer }
		};
	}
	void from_json(json const& j, Transform& t) {
		try {
			auto& pos = j.at("Position");
			auto& scl = j.at("Scale");

			if (j.contains("UIBasePosition"))
			{
				auto UIpos = j.at("UIBasePosition");
				t.UIbasePos = { UIpos.value("x", 0.0f), UIpos.value("y", 0.0f), UIpos.value("z", 0.0f) };
			}
			if (j.contains("UIBaseScale"))
			{
				auto UIscale = j.at("UIBaseScale");
				t.UIbaseScale = { UIscale.value("x", 0.0f), UIscale.value("y", 0.0f), UIscale.value("z", 0.0f) };
			}
			t.pos = { pos.value("x", 0.0f), pos.value("y", 0.0f), pos.value("z", 0.0f) };
			t.rot = j.value("Rotation", 0.0f);
			t.scale = { scl.value("x", 1.0f), scl.value("y", 1.0f) };

			// entity layer
			std::string l = j.value("Layer", "");
			if (l == "_Engine") { t.layer = LayerType::_ENGINE; }
			else if (l == "Background") { t.layer = LayerType::BACKGROUND; }
			else if (l == "Foreground") { t.layer = LayerType::FOREGROUND; }
			else if (l == "GameUI") { t.layer = LayerType::GAMEUI; }
			else if (l == "_Engine2") { t.layer = LayerType::_ENGINE2; }
			else { t.layer = LayerType::GAMELAYER; }
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// RIGIDBODY
	// --------------------------------------------------------------
	void to_json(json& j, RigidBody const& r) {
		float mass = 1.f;
		if (r.invMass != 0.f) {
			mass = 1.f / r.invMass;
		}
		j = {
			{"mass", mass},
			{"useGravity", r.useGravity},
			{"restitution", r.restitution},
			{"gravityMult", r.gravitymult},
			{"useRotate", r.useRotate}
		};
	}
	void from_json(json const& j, RigidBody& r) {
		try {
			float mass = r.invMass = 1.f;
			if (j.contains("mass")) {
				j.at("mass").get_to(mass);
				r.invMass = 1.f / mass;
			}
			float inertia{ mass };
			if (j.contains("inertia")) j.at("inertia").get_to(inertia);
			r.invInertia = 1.f / inertia;

			r.restitution = 0.5f;
			if (j.contains("restitution")) j.at("restitution").get_to(r.restitution);

			r.force = Force{};
			r.accel = Vec2{};
			r.vel = Vec2{};
			r.useGravity = false;
			j.at("useGravity").get_to(r.useGravity);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// SCRIPT
	// --------------------------------------------------------------
	void to_json(json& j, Script const& s) {
		j = {
			{"name", s.names}
		};

		//insert the changes to the scripts
		for (const auto& [scriptName, value] : s.modifiedValuesMap)
		{

			json entry;
			entry["varName"] = value.variableName;

			std::string varType{};
			switch (value.valueType)
			{
			case MonoValueType::MONO_INT:
			{
				varType = "int";
				entry["value"] = std::get<int>(value.value);
				break;
			}
			case MonoValueType::MONO_BOOL:
			{
				varType = "bool";
				entry["value"] = std::get<bool>(value.value);
				break;
			}
			case MonoValueType::MONO_FLOAT:
			{
				varType = "float";
				entry["value"] = std::get<float>(value.value);
				break;
			}
			case MonoValueType::MONO_STRING:
			{
				varType = "string";
				entry["value"] = std::get<std::string>(value.value);
				break;
			}
			case MonoValueType::MONO_VECTOR2:
			{
				varType = "vector2";
				Vec2 vec = std::get<Vec2>(value.value);
				entry["value"] = {
					{"x", vec.x},
					{"y", vec.y}
				};
				break;
			}
			case MonoValueType::MONO_VECTOR3:
			{
				varType = "vector3";
				Vec3 vec = std::get<Vec3>(value.value);
				entry["value"] = {
					{"x", vec.x},
					{"y", vec.y},
					{"z", vec.z}
				};
				break;
			}
			case MonoValueType::MONO_SCRIPT:
			{
				varType = "script";
				entry["value"] = "UNFINISHED SAVING LOGIC";
				break;
			}
			case MonoValueType::MONO_UNSUPPORTED:
			{
				varType = "unsupported";
				entry["value"] = "UNFINISHED SAVING LOGIC";
				break;
			}
			}

			entry["type"] = varType;

			j["modifier"][scriptName] += entry;
			
		}
	}
	void from_json(json const& j, Script& s) {
		try {
			//Push in all names of scripts
			for (const auto& names : j["name"])
			{
				s.names.push_back(names.get<std::string>());
			}

			//Check if contains modifiers
			if (j.contains("modifier") && j["modifier"].is_object())
			{
				//Push in all values modidfied
				for (const auto& [scriptname, modifierArr] : j["modifier"].items())
				{
					//Loop thorugh each modified value within this script (scriptname)
					for (const auto& mod : modifierArr)
					{
						ScriptModifiedValue value{};
						value.scriptName = scriptname;

						value.variableName = mod["varName"].get<std::string>();
						std::string strType = mod["type"].get<std::string>();

						if (strType == "int")
						{
							value.valueType = MonoValueType::MONO_INT;
							value.value = mod["value"].get<int>();
						}
						else if (strType == "bool")
						{
							value.valueType = MonoValueType::MONO_BOOL;
							value.value = mod["value"].get<bool>();
						}
						else if (strType == "float")
						{
							value.valueType = MonoValueType::MONO_FLOAT;
							value.value = mod["value"].get<float>();
						}
						else if (strType == "vector2")
						{
							value.valueType = MonoValueType::MONO_VECTOR2;
							Vec2 vec;
							vec.x = mod["value"]["x"].get<float>();
							vec.y = mod["value"]["y"].get<float>();
							value.value = vec;
						}
						else if (strType == "string")
						{
							value.valueType = MonoValueType::MONO_STRING;
							value.value = mod["value"].get<std::string>();
						}
						else if (strType == "vector3")
						{
							value.valueType = MonoValueType::MONO_VECTOR3;
							Vec3 vec;
							vec.x = mod["value"]["x"].get<float>();
							vec.y = mod["value"]["y"].get<float>();
							vec.z = mod["value"]["z"].get<float>();
							value.value = vec;
						}
						else if (strType == "script")
						{
							value.valueType = MonoValueType::MONO_SCRIPT;

						}
						else
							continue;

						s.modifiedValuesMap.insert(std::pair(scriptname, value));
					}

				}
			}
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// COLLIDER
	// --------------------------------------------------------------
	void to_json(json& j, Collider const& c) {
		j = {
			{"height", c.height},
			{"width", c.width},
			{"radius", c.radius},
			{"isStatic", c.isStatic}
		};
		// Shape
		if (ShapeType::AABB == c.shape) {
			j["shape"] = "AABB";
			std::cerr << "Collider shape is AABB\n";
		}
		else if (ShapeType::Circle == c.shape) {
			j["shape"] = "circle";
			std::cerr << "Collider shape is Circle\n";
		}
		// Collider type
		if (ColliderType::Solid == c.type) j["type"] = "solid";
		if (ColliderType::Trigger == c.type) j["type"] = "trigger";
	}
	void from_json(json const& j, Collider& c) {
		try {
			std::string shapeStr, typeStr;
			j.at("shape").get_to(shapeStr);
			j.at("type").get_to(typeStr);

			if (shapeStr == "AABB") {
				c.shape = ShapeType::AABB;
				std::cerr << "Collider become AABB\n";
			}
			else if (shapeStr == "Circle") {
				c.shape = ShapeType::Circle;
				std::cerr << "Collider become Circle\n";
			}

			if (typeStr == "Solid") c.type = ColliderType::Solid;
			else if (typeStr == "Trigger") c.type = ColliderType::Trigger;

			j.at("isStatic").get_to(c.isStatic);

			c.width = c.height = c.radius = 0.f;
			if (j.contains("width")) { j.at("width").get_to(c.width); }
			if (j.contains("height")) { j.at("height").get_to(c.height); }
			if (j.contains("radius")) { j.at("radius").get_to(c.radius); }

			// Optional flags to place player and enemies;
			c.isGrounded = j.value("isGrounded", false);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// RENDERABLE
	// --------------------------------------------------------------
	void to_json(json& j, Renderable const& r) {
		j = {
			{"isWireframe", r.isWireframe},
			{"materialName", r.materialName},
			{"meshName", r.meshName}
		};
	}
	void from_json(json const& j, Renderable& r) {
		try {
			j.at("meshName").get_to(r.meshName);
			j.at("materialName").get_to(r.materialName);

			if (j.contains("isWireframe"))
				j.at("isWireframe").get_to(r.isWireframe);
			else
				r.isWireframe = false;

			r.wasModified = true;
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// COLOR
	// --------------------------------------------------------------
	void to_json(json& j, Color const& c) {
		j = {
			{"r", c.r}, {"g", c.g}, {"b", c.b}, {"a", c.a}
		};
	}
	void from_json(json const& j, Color& c) {
		try {
			j.at("r").get_to(c.r);
			j.at("g").get_to(c.g);
			j.at("b").get_to(c.b);
			j.at("a").get_to(c.a);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// AUDIO
	// --------------------------------------------------------------
	void to_json(json& j, Audio const& a) {
		j = {
			{"audioKey", a.audioKey},
			{"isPlaying", a.isPlaying},
			{"loop", a.loop},
			{"playOnStart", a.playOnStart},
			{"volume", a.volume}
		};
	}
	void from_json(json const& j, Audio& a) {
		try {
			j.at("audioKey").get_to(a.audioKey);
			if (j.contains("volume"))
				j.at("volume").get_to(a.volume);
			j.at("loop").get_to(a.loop);
			j.at("playOnStart").get_to(a.playOnStart);
			j.at("isPlaying").get_to(a.isPlaying);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// CAMERA
	// --------------------------------------------------------------
	void to_json(json& j, Camera const& c) {
		j = {
			{"isMainCam", c.isMainCam},
			{"height", c.height},
			{"width", c.width},
			{"zoom", c.zoom},

			{"minX", c.minX}, {"minY", c.minY},
			{"maxX", c.maxX}, {"maxY", c.maxY},
			{"offsetX", c.offset.x}, {"offsetY", c.offset.y}
		};
	}
	void from_json(json const& j, Camera& c) {
		try {
			j.at("isMainCam").get_to(c.isMainCam);
			j.at("zoom").get_to(c.zoom);
			j.at("width").get_to(c.width);
			j.at("height").get_to(c.height);

			if (j.contains("minX")) j.at("minX").get_to(c.minX);
			if (j.contains("maxX")) j.at("maxX").get_to(c.maxX);
			if (j.contains("minY")) j.at("minY").get_to(c.minY);
			if (j.contains("maxY")) j.at("maxY").get_to(c.maxY);

			if (j.contains("offsetX")) j.at("offsetX").get_to(c.offset.x);
			if (j.contains("offsetY")) j.at("offsetY").get_to(c.offset.y);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// PLAYER CONTROLLER
	// --------------------------------------------------------------
	void to_json(json& j, PlayerController const& pc) {
		j = {
			{"jumpSpeed", pc.jumpSpeed},
			{"moveSpeed", pc.moveSpeed}
		};
	}
	void from_json(json const& j, PlayerController& pc) {
		try {
			j.at("jumpSpeed").get_to(pc.jumpSpeed);
			j.at("moveSpeed").get_to(pc.moveSpeed);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// HIERARCHY
	// --------------------------------------------------------------
	//void to_json(json& j, Hierarchy const& h) {
	//	bool parented = false;
	//	if (h.parent != INVALID_ENTITY) parented = true;

	//	std::vector<Entity> childPrefabs;
	//	for (std::pair<Entity, std::string> pair : h.children) {
	//		m_coord.GetSystem<EntityFactory>()->GetEntityPrefab();
	//	}
	//	j = {
	//		{ "isParented", parented},
	//		{ "children", h.children }
	//	};
	//}
	//void from_json(json const& j, Hierarchy& h);

	// --------------------------------------------------------------
	// ANIMATOR
	// --------------------------------------------------------------
	void to_json(json& j, Animator const& a) {
		j = {
			{"isLooping", a.isLooping},
			{"isPlaying", a.isPlaying},
			{"playbackSpeed", a.playbackSpeed},
			{"spriteSheetName", a.spriteSheetName}
		};
	}
	void from_json(json const& j, Animator& a) {
		try {
			j.at("spriteSheetName").get_to(a.spriteSheetName);
			j.at("playbackSpeed").get_to(a.playbackSpeed);
			j.at("isPlaying").get_to(a.isPlaying);
			j.at("isLooping").get_to(a.isLooping);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// FONT
	// --------------------------------------------------------------
	void to_json(json& j, Font const& f) {
		j = {
			{"fontSize", f.fontSize},
			{"fontType", f.fontType},
			{"text", f.text}
		};
		j["offsetPosition"] = {
			{"x", f.offsetPosition.x},
			{"y", f.offsetPosition.y},
			{"z", f.offsetPosition.z}
		};
		j["textColor"] = {
			{"r", f.textColor.x},
			{"g", f.textColor.y},
			{"b", f.textColor.z}
		};
	}
	void from_json(json const& j, Font& f) {
		try {
			j.at("fontType").get_to(f.fontType);
			j.at("text").get_to(f.text);
			j.at("fontSize").get_to(f.fontSize);
			auto& clr = j.at("textColor");
			f.textColor = { clr.value("r", 0.0f), clr.value("g", 0.0f), clr.value("b", 0.0f) };
			auto& offsetPos = j.at("offsetPosition");
			f.offsetPosition = { offsetPos.value("x", 0.0f), offsetPos.value("y", 0.0f), offsetPos.value("z", 0.0f) };
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// BUTTON INFO
	// --------------------------------------------------------------
	void to_json(json& j, ButtonInfo const& b) {
		j = { {"action", b.action} };
		if (ButtonAction::PLAYAUDIO == b.action) j["action"] = std::string{ "PlayAudio"};
		if (ButtonAction::LOADLEVEL == b.action) j["action"] = std::string{ "LoadLevel"};
		if (ButtonAction::OPENMENU == b.action) j["action"] = std::string{ "OpenMenu"};
		if (ButtonAction::NOACTION == b.action) j["action"] = std::string{ "NoAction"};

		if (ButtonType::SLIDER == b.type) j["type"] = std::string{ "Slider" };
		if (ButtonType::CLICKER == b.type) j["type"] = std::string{ "Clicker" };
	}
	void from_json(json const& j, ButtonInfo& b) {
		try {
			if (j["action"] == "PlayAudio") b.action = ButtonAction::PLAYAUDIO;
			// Menu actions
			if (j["action"] == "LoadLevel") { b.action = ButtonAction::LOADLEVEL; }
			if (j["action"] == "OpenMenu") { b.action = ButtonAction::OPENMENU; }
			if (j["action"] == "NoAction") { b.action = ButtonAction::NOACTION; }
			// Button type
			if (j["type"] == "Slider") { b.type = ButtonType::SLIDER; }
			if (j["type"] == "Clicker") { b.type = ButtonType::CLICKER; }

			b.target = j["target"];
		}
		catch(std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

	// --------------------------------------------------------------
	// SPRITE
	// --------------------------------------------------------------
	void to_json(json& j, Sprite const& s) {
		j = { {"textureName", s.textureName} };
	}
	void from_json(json const& j, Sprite& s) {
		try {
			j["textureName"].get_to(s.textureName);
		}
		catch (std::exception const& e) {
			std::cerr << "Invalid JSON format " << e.what() << '\n';
		}
	}

}	// Namespace NightSnack::Serializer

#endif
