// Fragment shader
#version 450 core

layout (location=0) out vec4 fFragColor;

in vec2 vTexCoord;

uniform vec3 uBaseColor;
uniform sampler2D uTex2d; // bound in Material::ApplyUniforms()
uniform bool uHasTexture;

void main() {

    if (uHasTexture){
        fFragColor = texture(uTex2d, vTexCoord) * vec4(uBaseColor, 1.0);
    } else{
        fFragColor = vec4(uBaseColor, 1.0);
    }

}