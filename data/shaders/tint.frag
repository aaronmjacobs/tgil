#version 330 core

uniform float uOpacity;
uniform vec3 uTint;

out vec4 color;

void main() {
   color = vec4(uTint, uOpacity);
}
