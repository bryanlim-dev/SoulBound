
#include "Serializer.h"

namespace NightSnack {

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
			if (j.contains("inertia")) {
				j.at("inertia").get_to(inertia);
			}
			r.invInertia = 1.f / inertia;

			r.restitution = 0.5f;
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
		j["Script"] = {
			{"name", s.names}
		};
	}
	void from_json(json const& j, Script& s) {
		try {
			for (const auto& names : j["name"])
			{
				s.names.push_back(names.get<std::string>());
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

	}
	void from_json(json const&, Collider&);

}	// Namespace NightSnack::Serializer
