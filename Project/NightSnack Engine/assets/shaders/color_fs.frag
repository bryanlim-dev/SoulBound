/******************************************************************************
 * File:        color_fs.frag
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Color Fragment Shader
 *
 * Description:
 * Color Fragment shader is responsible for determining the final color of each pixel.
 * This fragment shader only supports entities with a solid color.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Color Fragment shader
#version 450 core

layout (location=0) out vec4 fFragColor;

in vec3 vColor;

void main() {

    fFragColor = vec4(vColor, 1.0);
}