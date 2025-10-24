/******************************************************************************
 * File:        fragment_shader.frag
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Fragment Shader
 *
 * Description:
 * Fragment shader is responsible for determining the final color of each pixel.
 * It supports both textured and non-textured rendering.
 * If textured, sampled texture color is multiplied with base color.
 * If not textured, solid entity color is used instead.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Fragment shader
#version 450 core

layout (location=0) out vec4 fFragColor;

in vec2 vTexCoord;
in vec3 vInstanceColor;

uniform vec3 uBaseColor;
uniform vec3 uEntityColor;
uniform bool uUseInstancing;

uniform sampler2D uTex2d; // bound in Material::ApplyUniforms()

uniform bool uHasTexture;

void main() {

    if (uHasTexture){
        // Both non-instanced and instanced rendering will use uBaseColor
        fFragColor = texture(uTex2d, vTexCoord) * vec4(uBaseColor, 1.0);
    } else{
        if (uUseInstancing){
            fFragColor = vec4(vInstanceColor, 1.0);
        } else {
            fFragColor = vec4(uEntityColor, 1.0);
        }
    }
}