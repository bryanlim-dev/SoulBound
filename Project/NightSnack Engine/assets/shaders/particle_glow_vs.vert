/******************************************************************************
 * File:        particle_glow_vs.vert
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Particle Glow Vertex Shader
 *
 * Description:
 * Particle Glow Vertex shader is responsible for transforming vertices from model space to 
 * clip space, and passing texture coordinates to the fragment shader.
 * This vertex shader will only take care of particle entities. 
 *
 * Copyright © 2026 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Particle Glow Vertex shader
#version 450 core

layout (location=0) in vec3 aVertexPosition;
layout (location=1) in vec2 aVertexTexCoord;

// Shared by instanced and non-instanced rendering
uniform mat4 uProj;
uniform mat4 uView;
uniform bool uUseInstancing;
uniform vec4 uColor; // non instanced solid color
uniform int uInstanceOffset; 

// Non-instanced rendering
uniform mat4 uModelToWorld; // used for non-instancing rendering

struct InstanceData {
    vec4 pos_rot;
    vec4 scale;
    vec4 color;
    vec4 uvRect;
};

layout(std430, binding = 0) buffer InstanceBuffer {    
    InstanceData instances[];
};

out vec4 vColor; 
out vec2 vTexCoord;

mat4 computeModelMatrix(InstanceData data) {
    // scale
    mat4 S = mat4(1.0);
    S[0][0] = data.scale.x;
    S[1][1] = data.scale.y;

    // Rotation (Z-axis)
    float c = cos(data.pos_rot.w);
    float s = sin(data.pos_rot.w);
    mat4 R = mat4(1.0);
    R[0][0] = c;  R[0][1] = s;
    R[1][0] = -s; R[1][1] = c;

    // Translation
    mat4 T = mat4(1.0);
    T[3][0] = data.pos_rot.x;
    T[3][1] = data.pos_rot.y;
    T[3][2] = data.pos_rot.z;

    return T * R * S;
}

void main(void) {
    mat4 model;

    if (uUseInstancing) {
        int index = gl_InstanceID + uInstanceOffset;
        InstanceData data = instances[index];
        model = computeModelMatrix(data);
        vColor = data.color;
    } else {
        model = uModelToWorld;
        vColor = uColor;
    }

    gl_Position = uProj * uView * model * vec4(aVertexPosition, 1.0); 
    vTexCoord = aVertexTexCoord; // Pass texture coordinates for radial gradient calculation
}
