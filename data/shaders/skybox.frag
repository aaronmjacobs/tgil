#version 330 core

uniform samplerCube uSkybox;

in vec3 vSkyboxCoord;

out vec4 color;

void main() {
  color = texture(uSkybox, vSkyboxCoord);
}