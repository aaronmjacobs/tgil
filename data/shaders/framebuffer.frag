#version 330 core

uniform sampler2D uColor;
uniform sampler2D uDepth;
uniform float uBrightness = 1.0;

in vec2 vTexCoord;

out vec4 color;

void main() {
   color = texture(uColor, vTexCoord) * vec4(uBrightness);
}
