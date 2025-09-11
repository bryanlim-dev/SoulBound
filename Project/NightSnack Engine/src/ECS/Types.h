#pragma once
#include <bitset>
#include <set>
#include <cstddef>
#include <unordered_map>
#include <string>
#include <array>
#include <memory>		// std::shared_ptr
#include <typeindex>	// std::type_index

// Maximum number of entities in the system;
constexpr size_t MAX_ENTITIES = 5000;
// Maximum number of components;
constexpr size_t MAX_COMPONENTS = 32;

// Unique IDs;
using Entity = std::uint32_t;
using ComponentType = std::uint8_t;

using Signature = std::bitset<MAX_COMPONENTS>;