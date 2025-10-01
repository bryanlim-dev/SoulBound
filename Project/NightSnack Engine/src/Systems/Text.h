#pragma once

#include <unordered_map>
#include <iostream>
#include <memory>

#include "ECS/Coordinator.h"
#include "ECS/Component.h"
#include "ECS/ISystem.h"
#include "ECS/Entity.h"

using FontKey = size_t;

// System responsible for drawing Text Entities (debug version)
class TextSystem : public ISystem {
public:
    TextSystem() = default;

    explicit TextSystem(Coordinator& coord) : m_coord(&coord) {
        m_systemSignature.set(GetComponentTypeID<Position>());
        m_systemSignature.set(GetComponentTypeID<Rotation>());
        m_systemSignature.set(GetComponentTypeID<Text>());
    }

    void Init() {
        std::cout << "[TextSystem] Init called\n";
    }

    void Update(float, Coordinator&) override {
        for (auto entity : m_entities) {
            auto& pos = m_coord->GetComponent<Position>(entity);
            auto& rot = m_coord->GetComponent<Rotation>(entity);
            auto& txt = m_coord->GetComponent<Text>(entity);

            std::cout << "[TextSystem] Entity " << entity
                << " | Text: \"" << txt.text
                << "\" | Pos(" << pos.x << ", " << pos.y << ")"
                << " | Rot: " << rot.degrees << " degrees"
                << " | Size: " << txt.size << "\n";
        }
    }

    // Required by ISystem
    std::string GetName() const override { return "TextSystem"; }
    Signature GetSignature() const override { return m_systemSignature; }

private:
    Coordinator* m_coord = nullptr;
    Signature m_systemSignature{};
};
