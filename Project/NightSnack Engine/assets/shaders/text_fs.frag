/******************************************************************************
 * File:        text_fs.frag
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Text Fragment Shader
 *
 * Description:
 * Text Fragment shader is responsible for determining the final color of each pixel.
 * This Text Fragment Shader will only take care of text rendering. 
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/

#version 450 core

in vec2 texCoords;
out vec4 fFragColor;

uniform sampler2D text;
uniform vec3 uTextColor;

void main() {
    float alpha = texture(text, texCoords).r;
    fFragColor = vec4(uTextColor, alpha);
}