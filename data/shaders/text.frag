#version 330 core

uniform sampler2D uTexture;
uniform float uOpacity;
uniform vec3 uTint;

in vec2 vTexCoord;

out vec4 color;

void main() {
   color = vec4(uTint, texture(uTexture, vTexCoord).r * uOpacity);
}
