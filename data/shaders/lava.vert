#version 330 core

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec2 uUvScale = vec2(0.1);
uniform float uTime;

layout(location = 0) in vec3 aPosition;
layout(location = 2) in vec2 aTexCoord;

flat out vec2 vTexCoord;

void main() {
   vTexCoord = uUvScale * aPosition.xz / 100.0;

   float largeWave1 = sin((1.3 * aPosition.x + 2.0 * aPosition.z + uTime * 10.0) / 20.0) * 1.5;
   float largeWave2 = sin((-2.6 * aPosition.x + 0.8 * aPosition.z + uTime * 10.0) / 20.0) * 1.5;
   float smallWave = cos(1.7 * aPosition.z - 0.6 * aPosition.x + uTime * 2.0) / 2.0;
   float offset = largeWave1 + largeWave2 + smallWave;
   vec4 lPosition = vec4(aPosition.x, aPosition.y + offset, aPosition.z, 1.0);

   gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * lPosition;
}