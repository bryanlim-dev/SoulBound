/******************************************************************************
 * File:        debug_vertex_shader.vert
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Debug Vertex Shader
 *
 * Description:
 * Vertex shader is responsible for transforming vertices from model space to 
 * clip space.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Debug Vertex shader
#version 450 core

layout (location=0) in vec3 aVertexPosition;

// Shared by instanced and non-instanced rendering
uniform mat4 uDebugMVP;

void main(void) {
    gl_Position = uDebugMVP * vec4(aVertexPosition, 1.0); 
}
