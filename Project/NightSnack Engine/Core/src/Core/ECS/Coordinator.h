/******************************************************************************
 * File:        Coordinator.h
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%)
 *
 * Description:
 * This file declares class Coordinator, an interface that assists in interacting
 * with the ECS core.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#ifndef COORD_HPP
#define COORD_HPP

#include "Entity.h"
#include "Component.h"
#include "Layer.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "ISystem.h"
#include "Systems/Application/Application.h"
#include "Factory/Serializer.h"


namespace NightSnack {

	// Coordinator manages the ECS Managers;
	// Provides an interface that can access all of ECS Managers' functions and data;
	class Coordinator {

	public:
		/************************************************************************/
		/*!
		\brief
		Creates EntityManager, ComponentManager and SystemManager.
		*/
		/************************************************************************/
		void Initialize();

		/************************************************************************/
		/*!
		\brief
		Creates an entity and returns its ID (type Entity).

		\return
		Entity ID.
		*/
		/************************************************************************/
		Entity CreateEntity();

		/************************************************************************/
		/*!
		\brief
		Destroys an entity and by calling upon functionality of individual managers,
		will update all background linking (entities to components etc...).
		*/
		/************************************************************************/
		void DestroyEntity(Entity e);

		bool EntityIsAlive(Entity e) { return m_entityManager->EntityIsAlive(e); }

		/************************************************************************/
		/*!
		\brief
		Obtains a count of all living entities.

		\return
		Count of number of living entities.
		*/
		/************************************************************************/
		size_t GetEntityNum() { return m_entityManager->GetLivingEntities().size(); }	// number of living entities;

		/************************************************************************/
		/*!
		\brief
		Registers a component into the ECS core.
		*/
		/************************************************************************/
		template <typename T>
		void RegisterComponent();

		/************************************************************************/
		/*!
		\brief
		Adds a component to an entity.

		\param e
		Entity
		\param component
		Component desired to be added.
		*/
		/************************************************************************/
		template <typename T>
		void AddComponent(Entity e, T component);
		void AddComponent(Entity e, Collider c);

		/************************************************************************/
		/*!
		\brief
		Remove a component from an entity.

		\param e
		Entity
		*/
		/************************************************************************/
		template <typename T>
		void RemoveComponent(Entity e);

		/************************************************************************/
		/*!
		\brief
		Checks whether an entity has a specific component added.

		\param e
		Entity

		\return
		Returns true if the entity has the specified component, false otherwise.
		*/
		/************************************************************************/
		template<typename T>
		bool HasComponent(Entity e) const;

		/************************************************************************/
		/*!
		\brief
		Obtains a const reference to a specific component of an entity.
		(Accessor)

		\param e
		Entity

		\return
		Const reference to desired component.
		*/
		/************************************************************************/
		template <typename T>
		T const& GetComponent(Entity e) const;

		template <typename T>
		std::optional<T> const& GetComponentInChild(Entity e) const;
		template <typename T>
		std::optional<std::vector<T>> const& GetComponentsInChild(Entity e) const;
		template <typename T>
		std::optional<T> const& GetComponentInParent(Entity e) const;

		bool ParentEntity(Entity e, Entity parent) {
			if (!HasComponent<Hierarchy>(e) || !HasComponent<Hierarchy>(parent)) {
				std::cerr << "Entity " << e << " or " << parent 
					<< " does not have Hierarchy component, failed to add parent.\n";
				return false;
			}

			// Remove from previous parent if exists
			Hierarchy& h = ModifyComponent<Hierarchy>(e);
			if (h.parent != INVALID_ENTITY) {
				RemoveEntityChild(h.parent, e);
			}

			if (!AddChildToEntity(parent, e)) {
				std::cerr << "Failed to add child " << e << " to parent " << parent << '\n';
				return false;
			}
			h.parent = parent;
			return true;
		}

		bool RemoveEntityParent(Entity e, Entity parent) {
			if (!HasComponent<Hierarchy>(e)) {
				std::cerr << "Entity " << e << " does not have Hierarchy component, failed to remove parent.\n";
				return false;
			}
			if (!RemoveEntityChild(parent, e)) {
				std::cerr << "Failed to remove child " << e << " from parent " << parent << '\n';
				return false;
			}
			ModifyComponent<Hierarchy>(e).parent = INVALID_ENTITY;
			return true;
		}

		bool AddChildToEntity(Entity e, Entity child) {
			if (!HasComponent<Hierarchy>(e)) {
				std::cerr << "Entity " << e << " does not have Hierarchy component, failed to add child.\n";
				return false;
			}
			ModifyComponent<Hierarchy>(e).children.insert(child);
			return true;
		}

		bool RemoveEntityChild(Entity e, Entity child) {
			if (!HasComponent<Hierarchy>(e)) {
				std::cerr << "Entity " << e << " does not have Hierarchy component, failed to add child.\n";
				return false;
			}
			ModifyComponent<Hierarchy>(e).children.erase(child);
			return true;
		}

		/************************************************************************/
		/*!
		\brief
		Obtains a reference to a specific component of an entity.
		(Modifier)

		\param e
		Entity

		\return
		Reference to desired component.
		*/
		/************************************************************************/
		template <typename T>
		T& ModifyComponent(Entity e);
		// specialization for Renderable;
		template <>
		Renderable& ModifyComponent<Renderable>(Entity e);

		/************************************************************************/
		/*!
		\brief
		Obtains the signature of an entity.

		\param e
		Entity

		\return
		Reference to the Entity's signature.
		*/
		/************************************************************************/
		Signature& GetEntitySignature(Entity e) { return m_entityManager->GetSignature(e); }

		/************************************************************************/
		/*!
		\brief
		Obtains the set of living entities.

		\return
		Set of living entities.
		*/
		/************************************************************************/
		std::set<Entity> const& GetLivingEntities() const { return m_entityManager->GetLivingEntities(); }

		/************************************************************************/
		/*!
		\brief
		Obtains the living Entity Count of the Entity Manager;
		*/
		/************************************************************************/
		size_t GetLivingEntityCount() const { return m_entityManager->GetLivingEntityCount(); }

		/************************************************************************/
		/*!
		\brief
		Get all systems to update their set of entities by buffering an Entity's
		signature;
		*/
		/************************************************************************/
		void BufferEntitySignature(Entity e) { m_systemManager->EntitySignatureChanged(e, GetEntitySignature(e), GetComponent<Transform>(e).layer); }

		/************************************************************************/
		/*!
		\brief
		Registers a system into the core. Arguments are forwarded down till the
		individual constructors of each system.

		\param args
		May take any number of arguments, but must match the parameters of the
		actual system's constructor.

		\return
		Returns a shared_ptr to the system that was constructed.
		*/
		/************************************************************************/
		template <typename T, typename... Args>
		std::shared_ptr<T> RegisterSystem(Args&&... args);

		/************************************************************************/
		/*!
		\brief
		Obtains a shared_ptr of a specific system.

		\return
		shared_ptr of a specific system.
		*/
		/************************************************************************/
		template <typename T>
		std::shared_ptr<T> GetSystem() { return m_systemManager->GetSystem<T>(); }

		/************************************************************************/
		/*!
		\brief
		Obtains the height of the application window.
		*/
		/************************************************************************/
		int GetWindowHeight();
		/************************************************************************/
		/*!
		\brief
		Obtains the width of the application window.
		*/
		/************************************************************************/
		int GetWindowWidth();

		void ResetEverything();

		bool IsPaused() const noexcept { return m_paused; }
		void TogglePause() noexcept { m_paused = !m_paused; }
		void Pause() noexcept { m_paused = true; }
		void Unpause() noexcept { m_paused = false; }
		int steps = 0;

	private:
		// ECS Managers;
		std::unique_ptr<EntityManager> m_entityManager;
		std::unique_ptr<ComponentManager> m_componentManager;
		std::unique_ptr<SystemManager> m_systemManager;

		bool m_paused = true;
	};


	// ------------------------------------------------------------------------------------------------
	// TEMPLATE FUNCTIONS
	// ------------------------------------------------------------------------------------------------
	// Register a Component into the ComponentManager;
	template <typename T>
	void Coordinator::RegisterComponent() {
		// Call ComponentManager's member function to register a component
		// of type T into its unordered Map of ComponentArrays; Keys are type_index;
		m_componentManager->RegisterComponent<T>();
	}

	// Add a Component of type T to and Entity, updates the Entity's signature,
	// making sure to also inform the ComponentManager and SystemManager;
	template <typename T>
	void Coordinator::AddComponent(Entity e, T component) {

		if (HasComponent<T>(e)) {
			std::cerr << "Error: Entity " << e << " already has this component!!\n";
			return;
		}

		// Add component
		m_componentManager->AddComponent(e, component);

		// Obtain the Entity's component Signature;
		Signature sig = m_entityManager->GetSignature(e);
		// Modify the Signature to contain specified Component of type T;
		sig.set(GetComponentTypeID<T>(), true);

		// Update the actual Signature of Entity e;
		m_entityManager->SetSignature(e, sig);
		// Inform the SystemManager that Entity e's Signature has changed;
		m_systemManager->EntitySignatureChanged(e, sig, GetComponent<Transform>(e).layer);

		ModifyComponent<Transform>(e).prefabModified = true;
	}
	inline void Coordinator::AddComponent(Entity e, Collider c) {

		if (HasComponent<Collider>(e)) {
			std::cerr << "Error: Entity " << e << " already has this component!!\n";
			return;
		}
		
		// Collider default size
		Transform const& t = GetComponent<Transform>(e);
		//if (HasComponent<Renderable>(e)) {
		//	Renderable const& r = GetComponent<Renderable>(e);
		//	if (r.meshName == "circle")
		//		c.shape = ShapeType::Circle;
		//	else
		//		c.shape = ShapeType::AABB;
		//}
		if (c.width == 0.f) { c.width = t.scale.x; }
		if (c.height == 0.f) { c.height = t.scale.y; }
		if (c.radius == 0.f) { c.radius = (t.scale.x + t.scale.y) / 2.f; }

		// Add component
		m_componentManager->AddComponent(e, c);

		// Obtain the Entity's component Signature;
		Signature sig = m_entityManager->GetSignature(e);
		// Modify the Signature to contain specified Component of type T;
		sig.set(GetComponentTypeID<Collider>(), true);

		// Update the actual Signature of Entity e;
		m_entityManager->SetSignature(e, sig);
		// Inform the SystemManager that Entity e's Signature has changed;
		m_systemManager->EntitySignatureChanged(e, sig, GetComponent<Transform>(e).layer);

		ModifyComponent<Transform>(e).prefabModified = true;
	}

	template <typename T>
	void Coordinator::RemoveComponent(Entity e) {

		if (!HasComponent<T>(e)) {
			std::cerr << "Error: Entity " << e << " does not have this component, cannot remove!!\n";
			return;
		}

		// Remove component;
		m_componentManager->RemoveComponent<T>(e);

		// Obtain the Entity's component Signature;
		Signature sig = m_entityManager->GetSignature(e);
		// Modify the Signature to remove specified Component of type T;
		sig.set(GetComponentTypeID<T>(), false);

		// Update the actual Signature of Entity e;
		m_entityManager->SetSignature(e, sig);
		// Inform the SystemManager that Entity e's Signature has changed;
		m_systemManager->EntitySignatureChanged(e, sig, GetComponent<Transform>(e).layer);

		ModifyComponent<Transform>(e).prefabModified = true;
	}

	template <typename T>
	T const& Coordinator::GetComponent(Entity e) const {
		return m_componentManager->GetComponent<T>(e);
	}

	template <typename T>
	std::optional<T> const& Coordinator::GetComponentInChild(Entity e) const {
		
		// Edge case, no hierarchy component;
		if (!HasComponent<Hierarchy>(e)) {
			std::cerr << "[Coordinator] Error: Entity " << e 
				<< " does not have Hierarchy Component, unable to get Child component.\n";
			return std::nullopt;
		}

		// Return immediately if current has component;
		if (HasComponent<T>(e)) return GetComponent<T>(e);

		// Iterate through children stored in Transform component;
		Hierarchy const& h = GetComponent<Hierarchy>(e);
		if (h.children.empty()) {
			std::cerr << "[Coordinator] Error: Entity " << e
				<< " does not have any Children.\n";
			return std::nullopt;
		}
		for (Entity c : h.children) {
			if (!HasComponent<T>(c)) continue;
			return GetComponent<T>(c);
		}
		// No Children contains requested component;
		std::cerr << "[Coordinator] Error: Entity " << e
			<< " does not have Children with requested component.\n";
		return std::nullopt;
	}

	template <typename T>
	std::optional<std::vector<T>> const& Coordinator::GetComponentsInChild(Entity e) const {

		// Edge case, no hierarchy component;
		if (!HasComponent<Hierarchy>(e)) {
			std::cerr << "[Coordinator] Error: Entity " << e
				<< " does not have Transform Component, unable to get Child components.\n";
			return std::nullopt;
		}

		// Return vector;
		std::vector<std::pair<Entity, T>> ret{};
		if (HasComponent<T>(e)) ret.emplace_back({ e, GetComponent<T>(e) });

		// Iterate through children stored in Transform component;
		Hierarchy const& h = GetComponent<Hierarchy>(e);
		if (h.children.empty()) {
			std::cerr << "[Coordinator] Error: Entity " << e
				<< " does not have any Children.\n";
			return std::nullopt;
		}
		for (Entity c : h.children) {
			if (!HasComponent<T>(c)) continue;
			ret.emplace_back({ c,GetComponent<T>(c) });
		}
		if (ret.empty()) {
			// No Children contains requested component;
			std::cerr << "[Coordinator] Error: Entity " << e
				<< " does not have Children with requested component.\n";
		}
		return std::nullopt;
	}

	template <typename T>
	std::optional<T> const& Coordinator::GetComponentInParent(Entity e) const {

		if (HasComponent<T>(e)) return GetComponent<T>(e);

		// Edge case, no hierarchy component;
		if (!HasComponent<Hierarchy>(e)) {
			std::cerr << "[Coordinator] Error: Entity " << e
				<< " does not have Hierarchy Component, unable to get Parent component.\n";
			return std::nullopt;
		}
		Hierarchy const& h = GetComponent<Hierarchy>(e);
		if (h.parent == INVALID_ENTITY) {
			std::cerr << "[Coordinator] Error: Entity " << e
				<< " does not have a Parent, returning empty component.\n";
			return std::nullopt;
		}
		if (HasComponent<T>(h.parent)) return GetComponent<T>(h.parent);

		return GetComponentInParent(h.parent);
	}

	template <typename T>
	bool Coordinator::HasComponent(Entity e) const {
		return m_componentManager->HasComponent<T>(e);
	}

	template <typename T>
	T& Coordinator::ModifyComponent(Entity e) {

		return m_componentManager->GetComponent<T>(e);
	}
	template <>	// specialization for Renderable;
	inline Renderable& Coordinator::ModifyComponent<Renderable>(Entity e) {

		auto& r = m_componentManager->GetComponent<Renderable>(e);
		// To enable GraphicsSystem to know that RenderCache must be updated
		r.wasModified = true;
		return r;
	}

	template <typename T, typename... Args>
	std::shared_ptr<T> Coordinator::RegisterSystem(Args&&... args) {
		return m_systemManager->RegisterSystem<T>(std::forward<Args>(args)...);
	}

}	// NightSnack

#endif // COORD_HPP