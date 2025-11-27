/******************************************************************************
 * File:        color_vs.vert
 * Project:     NightSnack
 * Author(s):	Goh Bei Lai (Primary, 100%) - Created Color Vertex Shader
 *
 * Description:
 * Color Vertex shader is responsible for transforming vertices from model space to 
 * clip space, and passing texture coordinates to the fragment shader.
 * This vertex shader will only take care of entities of solid colors.
 *
 * Copyright © 2025 DigiPen Institute of Technology Singapore.
 * All rights reserved.
 ******************************************************************************/
// Color Vertex shader
#version 450 core

layout (location=0) in vec3 aVertexPosition;

// Shared by instanced and non-instanced rendering
uniform mat4 uProj;
uniform mat4 uView;
uniform bool uUseInstancing;
uniform vec3 uColor; // non instanced solid color
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

out vec3 vColor; 

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
        vColor = data.color.xyz;
    } else {
        model = uModelToWorld;
        vColor = uColor;
    }

    gl_Position = uProj * uView * model * vec4(aVertexPosition, 1.0); 
}
