/******************************************************************************
 * File:        particle_glow_fs.frag
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Particle Glow Fragment Shader
 *
 * Description:
 * Particle Glow Fragment shader is responsible for determining the final color of each pixel.
 * This fragment shader only supports Particle entities. 
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Particle Glow Fragment shader
#version 450 core

layout (location=0) out vec4 fFragColor;

in vec4 vColor;
in vec2 vTexCoord;

uniform float uGlowIntensity;
uniform float uGlowFalloff;

void main() {

    vec4 particleColor = vColor;

    // calculate distance from center
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(vTexCoord, center);

    // normalize distance
    float normalizedDist = dist / 0.707;
    normalizedDist = clamp(normalizedDist, 0.0, 1.0);

    // create 2 layer glow for bloom effect
    float coreGlow = 1.0 - pow(normalizedDist, uGlowFalloff * 1.8);
    coreGlow = clamp(coreGlow, 0.0, 1.0);

    // outer glow
    float outerGlow = 1.0 - pow(normalizedDist, uGlowFalloff * 0.6);
    outerGlow = clamp(outerGlow, 0.0, 1.0);

    // combine glows with different intensities
    float finalAlpha = max(coreGlow, outerGlow * 0.4);
    finalAlpha *= uGlowIntensity;

    // brighten the center for bloom effect
    float brightness = 1.0 + (1.0 - normalizedDist) * 0.6;
    vec3 finalColor = particleColor.rgb * brightness;

    fFragColor = vec4(finalColor, particleColor.a * finalAlpha);
}