#version 330 core

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

in vec3 aPosition;
in vec2 aTexCoord;

out vec2 vTexCoord;

void main() {
   // Transforms
   vec4 lPosition = uModelMatrix * vec4(aPosition.xyz, 1.0);
   gl_Position = uProjMatrix * uViewMatrix * lPosition;

   // Texturing
   vTexCoord = aTexCoord;
}