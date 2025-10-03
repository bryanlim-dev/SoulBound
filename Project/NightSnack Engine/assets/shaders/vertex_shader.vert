/******************************************************************************
 * File:        vertex_shader.vert
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Vertex Shader
 *
 * Description:
 * Vertex shader is responsible for transforming vertices from model space to 
 * clip space, and passing texture coordinates to the fragment shader.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Vertex shader
#version 450 core

layout (location=0) in vec3 aVertexPosition;
layout (location=1) in vec2 aTexCoord;

uniform mat4 uMVP;

out vec2 vTexCoord; // give texcoord to fragment shader

void main(void) {
    gl_Position = uMVP * vec4(aVertexPosition, 1.0); 
    vTexCoord = aTexCoord;
}
