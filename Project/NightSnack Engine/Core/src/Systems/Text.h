#pragma once

#include <unordered_map>
#include <iostream>
#include <memory>

#include "Core/ECS/Coordinator.h"
#include "Core/ECS/Entity.h"
#include "ResourceManager/ResourceManager.h"
#include <glm/ext/vector_int2.hpp>
#include <Graphics/Shader.h>

namespace NightSnack {

    class TextSystem : public ISystem {
    public:
        TextSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
            m_systemSignature.set(GetComponentTypeID<Font>());
            m_systemSignature.set(GetComponentTypeID<Transform>());
            Init();
        }

        struct Character {
            GLuint textureID;
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };

        void Init();
        
        void LoadFontGlyphs(size_t fontKey, const std::string& name);

        void Update(int steps) override;

        void Draw(Entity camera, bool isGameCamera);

        void RenderText(std::string text, float x, float y, float scale, glm::vec3 color, 
            const glm::mat4& proj, const glm::mat4& view, const std::string& fontName);

        // Required by ISystem
        std::string GetName() const override { return "FontSystem"; }
        Signature GetSignature() const override { return m_systemSignature; }

    private:

        std::unordered_map<std::string, size_t> m_fonts; // [fontName, fontKey]
        std::unordered_map<std::string, std::unordered_map<char, Character>> m_fontCharacters;
        Shader* m_shader = nullptr; // Only 1 text shader needed
        float m_windowWidth{}, m_windowHeight{};
        unsigned int m_vao{}, m_vbo{};

        Coordinator& m_coord;
        EventDispatcher* m_dispatcher = nullptr;
        Signature m_systemSignature{};
    };

}