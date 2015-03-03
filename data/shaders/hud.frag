#version 330 core

uniform sampler2D uTexture;
uniform vec2 uFill = vec2(1.0);
uniform vec3 uTint = vec3(1.0);
uniform float uOpacity = 0.5;

in vec2 vTexCoord;

out vec4 color;

void main() {
   if (vTexCoord.x > uFill.x || vTexCoord.y > uFill.y) {
      discard;
   }

   color = texture(uTexture, vTexCoord) * vec4(uTint, uOpacity);
}
