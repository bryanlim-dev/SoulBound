// Vertex shader
#version 450 core

layout (location=0) in vec2 aVertexPosition;
layout (location=1) in vec2 aTexCoord;

uniform mat4 uMVP;

out vec2 vTexCoord; // give texcoord to fragment shader

void main(void) {
    gl_Position = uMVP * vec4(aVertexPosition, 0.0, 1.0); // z=0
    vTexCoord = aTexCoord;
}
