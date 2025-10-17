/******************************************************************************
 * File:        Material.cpp
 * Project:     NightSnack
 * Author(s):   Goh Bei Lai (Primary, 100%) - Created Material class
 *
 * Description:
 * The Material class encapsulates shader and texture data.
 * It manages base color, texture binding.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#include "Material.h"

namespace NightSnack {

    Material::Material(Shader* shader, Texture* texture)
        : m_shader(shader), m_texture(texture) {

        // Enable alpha blending only if there is a texture
        // And if the texture is a png format too
        m_enableAlphaBlending = m_texture && m_texture->HasAlpha();
    }

    Shader* Material::GetShader() const {
        return m_shader;
    }

    Texture* Material::GetTexture() const {
        return m_texture;
    }

    void Material::SetBaseColor(float r, float g, float b, float a) {
        m_baseColor = glm::vec4{ r, g, b, a };
    }

    void Material::ApplyUniforms(bool wireframe) {

        m_shader->Use();

        m_shader->SetVec3("uBaseColor", m_baseColor);

        // Textures will be binded if it is not in wireframe mode
        if (m_texture && !wireframe) {
            m_texture->Bind(); // calls glBindTexture(GL_TEXTURE_2D, m_id)
            m_shader->SetInt("uTex2d", 0);
            m_shader->SetBool("uHasTexture", true);
        }
        else {
            m_shader->SetBool("uHasTexture", false);
        }

        // Enable alpha blending for textures with .png format
        if (m_enableAlphaBlending) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Do not write depth when rendering transparent PNGs
            // So that the transparent parts don't block later objects
            glDepthMask(GL_FALSE);
        }
        else {
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }
    }

}   // NightSnack
