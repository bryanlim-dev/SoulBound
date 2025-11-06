#pragma once

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

#include <filesystem>
#include <iostream>
#include <string>

namespace NightSnack::Scripting {

	MonoDomain* s_RootDomain;

	//void InitMono() {
	//	mono_set_assemblies_path("../bin");

	//	MonoDomain* root = mono_jit_init("ScriptRuntime");
	//	if (root == nullptr) {
	//		std::cerr << "MonoDomain* is null, InitMono() failed.\n";
	//		return;
	//	}
	//	s_RootDomain = root;

	//	// Create an App Domain
	//	s_AppDomain = mono_domain_create_appdomain((char*)"AppDomain", nullptr);
	//	mono_domain_set(s_AppDomain, true);
	//}
}
