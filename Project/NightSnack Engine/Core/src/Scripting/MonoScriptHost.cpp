#pragma once

#include "Scripting/MonoScriptHost.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <optional>

namespace NightSnack::Scripting {

    MonoScriptHost::MonoScriptHost()
	: m_rootDomain(nullptr)
	, m_appDomain(nullptr)
	, m_assembly(nullptr)
	, m_assemblyImage(nullptr)
	, m_initialized(false) {
      InitMono();
    }

    bool MonoScriptHost::InitMono() {
		if (m_initialized) {
			std::cerr << "Mono already initialized.\n";
			return true;
		}

		std::cerr << "[Mono] Initializing..." << std::endl;

		mono_set_assemblies_path("../bin");

		MonoDomain* root = mono_jit_init("ScriptRuntime");
		if (root == nullptr) {
			std::cerr << "MonoDomain* is null, InitMono() failed.\n";
			return false;
		}
		m_rootDomain = root;

		// Create an App Domain
		m_appDomain = mono_domain_create_appdomain((char*)"AppDomain", nullptr);
		mono_domain_set(m_appDomain, true);

		return true;
    }

    void MonoScriptHost::Shutdown() {
        if (!m_initialized) return;

        std::cerr << "Mono shutting down...\n";

        // Clear script instances
        m_scriptInstances.clear();
        m_classCache.clear();

        // Unload app domain
        if (m_appDomain) {
            mono_domain_set(mono_get_root_domain(), false);
            mono_domain_unload(m_appDomain);
            m_appDomain = nullptr;
        }

        // Cleanup JIT
        if (m_rootDomain) {
            mono_jit_cleanup(m_rootDomain);
            m_rootDomain = nullptr;
        }

        m_assembly = nullptr;
        m_image = nullptr;
        m_initialized = false;

        std::cerr << "Mono shutdown complete.\n";
    }

    static std::optional<std::vector<char>> ReadBytesFromFile(std::string const& filepath, size_t& outSize) {
        outSize = 0;
        
        std::ifstream ifs(filepath, std::ios::binary | std::ios::ate);
        if (!ifs) {
            std::cerr << "Failed to open file: " << filepath << '\n';
            return std::nullopt;
        }
        size_t end = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        size_t size = end - ifs.tellg();

        if (size == 0) {
            std::cerr << "File: " << filepath << " is empty.\n";
            return std::nullopt;
        }
        std::vector<char> buf(size);
        ifs.read(buf.data(), size);
        ifs.close();

        outSize = size;
        return buf;
    }

    bool MonoScriptHost::LoadAssembly(const std::string& asspath) {

        if (!m_initialized) {
            std::cerr << "Mono is uninitialized, unable to load assembly.\n";
            return false;
        }
        // Open assembly;
        m_assembly = mono_domain_assembly_open(m_appDomain, asspath.c_str());
        if (!m_assembly) {
            std::cerr << "Mono failed to load assembly: " << asspath << '\n';
            return false;
        }
        // Get assembly image
        m_image = mono_assembly_get_image(m_assembly);
        if (!m_image) {
            std::cerr << "Mono failed to get assembly image\n";
            return false;
        }
        //std::cout << "Mono Assembly loaded successfully\n";
        return true;
    }

}   // NightSnack::Scripting