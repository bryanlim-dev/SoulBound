// Fragment shader code goes here...
#version 450 core

//layout (location=0) in vec3 vColor;
layout (location=0) out vec4 fFragColor;

uniform vec3 uBaseColor;

void main() {
    fFragColor = vec4(uBaseColor, 1.0);
}