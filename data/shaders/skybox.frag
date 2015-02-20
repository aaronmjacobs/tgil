#version 120

uniform samplerCube uSkybox;

varying vec3 vSkyboxCoord;

void main() {
  gl_FragColor = textureCube(uSkybox, vSkyboxCoord);
}