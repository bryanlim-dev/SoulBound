/******************************************************************************
 * File:        debug_fragment_shader.frag
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Debug Fragment Shader
 *
 * Description:
 * Fragment shader is responsible for determining the final color of each pixel.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Debug Fragment shader
#version 450 core

layout (location=0) out vec4 fFragColor;

uniform vec3 uEntityColor;

void main() {
    fFragColor = vec4(uEntityColor, 1.0);
}