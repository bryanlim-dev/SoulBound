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

// Shared by instanced and non-instanced rendering
uniform mat4 uProj;
uniform mat4 uView;
uniform bool uUseInstancing;

// Non-instanced rendering
uniform mat4 uModelToWorld; // used for non-instancing rendering

// Instanced rendering
struct InstanceData {
    vec3 position;
    float rotation;
    vec2 scale;
    vec3 color;
    float padding;
};
layout(std430, binding = 0) buffer InstanceBuffer {    
    InstanceData instances[];
};

out vec2 vTexCoord; // give texcoord to fragment shader
out vec3 vInstanceColor; 

mat4 computeModelMatrix(InstanceData data) {
    // scale
    mat4 S = mat4(1.0);
    S[0][0] = data.scale.x;
    S[1][1] = data.scale.y;

    // Rotation (Z-axis)
    float c = cos(data.rotation);
    float s = sin(data.rotation);
    mat4 R = mat4(1.0);
    R[0][0] = c;  R[0][1] = s;
    R[1][0] = -s; R[1][1] = c;

    // Translation
    mat4 T = mat4(1.0);
    T[3][0] = data.position.x;
    T[3][1] = data.position.y;
    T[3][2] = data.position.z;

    return T * R * S;
}

void main(void) {
    mat4 model;

    if (uUseInstancing) {
        InstanceData data = instances[gl_InstanceID];
        model = computeModelMatrix(data);
        gl_Position = vec4(vec2(uProj * uView * model * vec4(aVertexPosition, 1.0)), 0.0, 1.0);
        vInstanceColor = data.color;
    } else {
        model = uModelToWorld;
        gl_Position = uProj * uView * model * vec4(aVertexPosition, 1.0); 
        vInstanceColor = vec3(1.0);
    }

    vTexCoord = aTexCoord;
}
