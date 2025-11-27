/******************************************************************************
 * File:        texture_fs.frag
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Texture Fragment Shader
 *
 * Description:
 * Texture Fragment shader is responsible for determining the final color of each pixel.
 * This fragment shader only supports entities with textures.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Texture Fragment shader
#version 450 core

layout (location=0) out vec4 fFragColor;

in vec2 vTexCoord;

uniform bool uUseInstancing;
uniform sampler2D uTex2d; // bound in Material::ApplyUniforms()
uniform bool uIsWireframe;

void main() {
    
    if (uIsWireframe) {
        fFragColor = vec4(1.0, 1.0, 1.0, 1.0); // white color by default
    } else {
        fFragColor = texture(uTex2d, vTexCoord);
    }
}