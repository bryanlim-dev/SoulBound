/******************************************************************************
 * File:        Text.h
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Text System
 *
 * Description:
 * This file contains the declarations of the Text System.
 * Loading of font files using the ResourceManager.
 * Preparing the glyph textures for ASCII characters.
 * Rendering of text belonging to entities in in-game and UI layers.
 *
 * Copyright � 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

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
        /************************************************************************/
        /*!
        \brief
        Constructor for the TextSystem
        Sets up the system signatures so that this system only operates on entities
        that contain components (Font, Transform)

        \param[in] coord
        Reference to the ECS Coordinator

        \param[in] dispatcher
        Reference to the event dispatcher used for Input Events
        */
        /************************************************************************/
        TextSystem(Coordinator& coord, EventDispatcher& dispatcher) : m_coord{ coord }, m_dispatcher{ &dispatcher } {
            m_systemSignature.set(GetComponentTypeID<Font>());
            m_systemSignature.set(GetComponentTypeID<Transform>());
            Init();

            AddLayer(LayerType::GAMELAYER);
            AddLayer(LayerType::GAMEUI);
        }

        struct Character {
            GLuint textureID;
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };

        /************************************************************************/
        /*!
        \brief
        Initializes the TextSystem. 
        Loads the fonts using ResourceManager.
        Generates glyph textures for the fonts.
        Set up OpenGL VAO/VBO for rendering text quads.
        Link the text shader from the GraphicsSystem.
        */
        /************************************************************************/
        void Init();
        
        /************************************************************************/
        /*!
        \brief
        Loads and generate glyph textures for a font.
        Loads ASCII characters from 0-127.
        Generate OpenGL texture for each glyph and store the resulting character and texture IDs.

        \param[in] fontKey
        Key to reference the font from the ResourceManager

        \param[in] name
        std::string key representing the name of the font, to be used for m_fontCharacters
        */
        /************************************************************************/
        void LoadFontGlyphs(size_t fontKey, const std::string& name);

        /************************************************************************/
        /*!
        \brief
        Finds the main active camera entity and call Draw()

        \param[in] steps
        The number of steps in the simulation
        */
        /************************************************************************/
        void Update(int steps) override;

        /************************************************************************/
        /*!
        \brief
        Renders all text entities from visible layers

        \param[in] camera
        The active camera entity

        \param[in] isGameCamera
        Bool representing whether the camera is the game camera or not
        */
        /************************************************************************/
        void Draw(Entity camera, bool isGameCamera);

        /************************************************************************/
        /*!
        \brief
        Renders a single line of text.
        Iterates through each character in the text and renders textured quads representing each glyph

        \param[in] text
        The string to render

        \param[in] x
        The x position in screen or world space, depending on the entity's layer

        \param[in] y
        The y position in screen or world space, depending on the entity's layer

        \param[in] scale
        The font size scaling factor

        \param[in] color
        The text color

        \param[in] proj
        The projection matrix

        \param[in] view
        The view matrix

        \param[in] fontName
        The font's name
        */
        /************************************************************************/
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