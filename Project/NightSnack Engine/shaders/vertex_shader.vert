// Vertex shader code goes here...
#version 450 core

// 0 is vertex attribute index that is associated with per-vertex
// position coordinates in VBO; this association was performed by
// this call: glEnableVertexArrayAttrib(vaoid, 0);
layout (location=0) in vec3 aVertexPosition;

void main(void) {
    gl_Position = vec4(aVertexPosition, 1.0);
}
