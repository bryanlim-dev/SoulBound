#version 450 core

in vec2 texCoords;
out vec4 fFragColor;

uniform sampler2D text;
uniform vec3 uTextColor;

void main() {
    float alpha = texture(text, texCoords).r;
    fFragColor = vec4(uTextColor, alpha);
}