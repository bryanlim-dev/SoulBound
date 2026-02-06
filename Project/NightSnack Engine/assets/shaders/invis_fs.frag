/******************************************************************************
 * File:        invis_fs.frag
 * Project:     NightSnack
 * Author(s):	Chewn Thing Kwan (Primary, 100%) - Created Invisible Fragment Shader
 *
 * Description:
 * Invisible Fragment shader is responsible for determining the final color of each pixel.
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

// Color Fragment shader
#version 450 core

layout (location=0) out vec4 fFragColor;

in vec4 vColor;

void main() {
    fFragColor = vec4(0.f);
}