/******************************************************************************
 * File:        Material.h
 * Project:     NightSnack
 * Author(s):
 *
 * Description:
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

class Material {
public:
    Material(Shader* shader, Texture* texture = nullptr);

    Shader* GetShader() const;
    Texture* GetTexture() const;

    void SetBaseColor(float r, float g, float b, float a);

    void ApplyUniforms(bool wireframe);

private:
    Shader* m_shader;
    Texture* m_texture;
    glm::vec4 m_baseColor{ 1.f, 1.f, 1.f, 1.f};
    bool m_enableAlphaBlending{ false };
};
