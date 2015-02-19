#version 120

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord;

varying vec3 vWorldPosition;
varying vec3 vNormal;
varying vec2 vTexCoord;

void main() {
   // Transforms
   vec4 lPosition = uModelMatrix * vec4(aPosition.xyz, 1.0);
   vWorldPosition = lPosition.xyz;
   gl_Position = uProjMatrix * uViewMatrix * lPosition;

   // Calculate the relative normal
   vec4 lNormal = vec4(aNormal.xyz, 0.0);
   lNormal = uNormalMatrix * lNormal;
   vNormal = lNormal.xyz;

   // Texturing
   vTexCoord = aTexCoord;
}