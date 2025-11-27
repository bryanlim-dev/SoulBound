/******************************************************************************
 * File:        text_vs.vert
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Text Vertex Shader
 *
 * Description:
 * Text Vertex shader is responsible for determining the final color of each pixel.
 * This Text Vertex Shader will only take care of text rendering. 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#version 450 core

layout (location = 0) in vec4 vertex;

out vec2 texCoords;

uniform mat4 proj;
uniform mat4 view;

void main() {
    gl_Position = proj * view * vec4(vertex.xy, 0.0, 1.0);
    texCoords = vertex.zw;
}