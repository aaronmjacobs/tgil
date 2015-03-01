#version 330 core

uniform float uOpacity = 0.5;
uniform vec3 uTint = vec3(1.0);

out vec4 color;

void main() {
   color = vec4(uTint, uOpacity);
}
