#version 330 core

uniform sampler2D uTexture;
uniform float opacity = 0.5;
uniform vec3 tint = vec3(1.0);

in vec2 vTexCoord;

out vec4 color;

void main() {
   color = texture(uTexture, vTexCoord) * vec4(tint, opacity);
}
