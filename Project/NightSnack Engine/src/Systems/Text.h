#pragma once

#include <unordered_map>
#include <iostream>
#include <memory>

#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Entity.h"

namespace NightSnack {

    using FontKey = size_t;

    // System responsible for drawing Text Entities (debug version)
    class TextSystem : public ISystem {
    public:
        TextSystem() = default;

        TextSystem(Coordinator& coord) : m_coord(&coord) {
            //m_systemSignature.set(GetComponentTypeID<Position>());
            //m_systemSignature.set(GetComponentTypeID<Rotation>());
            //m_systemSignature.set(GetComponentTypeID<Text>());
        }

        void Init() {
            std::cout << "[TextSystem] Init called\n";
        }

        //void Update(float, Coordinator&) override {
        void Update(int steps) override {
            /*for (auto entity : m_entities) {
                auto& pos = m_coord->GetComponent<Position>(entity);
                auto& rot = m_coord->GetComponent<Rotation>(entity);
                auto& txt = m_coord->GetComponent<Text>(entity);

                std::cout << "[TextSystem] Entity " << entity
                    << " | Text: \"" << txt.text
                    << "\" | Pos(" << pos.x << ", " << pos.y << ")"
                    << " | Rot: " << rot.degrees << " degrees"
                    << " | Size: " << txt.size << "\n";
            }*/

            //for (Entity e : m_entities) {
            //    //if (m_coord.HasComponent<Text>(e)) {
            //    auto& pos = m_coord->GetComponent<Transform>(e).pos;
            //    auto& rot = m_coord->GetComponent<Transform>(e).rot;
            //    auto& txt = m_coord->GetComponent<Text>(e);

            //    glm::vec4 clr{ txt.color.r, txt.color.g, txt.color.b, txt.color.a };

            //    ResourceManager* rm = ResourceManager::GetInstance();
            //    FT_Face face = rm->GetFont(m_coord->GetSystem<GraphicsSystem>()->GetFontKey(txt.fontName));

            //    if (face) {
            //        RenderText(face, txt.text, { pos.x, pos.y }, txt.size, clr, rot.degrees);
            //        std::cout << "have face\n";
            //    } else
            //        std::cout << "no face\n";
            //    //}
            //}
        }

        // Required by ISystem
        std::string GetName() const override { return "TextSystem"; }
        Signature GetSignature() const override { return m_systemSignature; }

    private:
        Coordinator* m_coord = nullptr;
        Signature m_systemSignature{};
    };

}