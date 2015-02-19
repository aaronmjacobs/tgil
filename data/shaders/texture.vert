#version 120

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

attribute vec3 aPosition;
attribute vec2 aTexCoord;

varying vec2 vTexCoord;

void main() {
   // Transforms
   vec4 lPosition = uModelMatrix * vec4(aPosition.xyz, 1.0);
   gl_Position = uProjMatrix * uViewMatrix * lPosition;

   // Texturing
   vTexCoord = aTexCoord;
}