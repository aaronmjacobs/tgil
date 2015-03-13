#version 330 core

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uShadowProj;
uniform mat4 uShadowView;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vWorldPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vShadowCoord;

void main() {
   // Transforms
   vec4 lPosition = uModelMatrix * vec4(aPosition.xyz, 1.0);
   vWorldPosition = lPosition.xyz;
   gl_Position = uProjMatrix * uViewMatrix * lPosition;

   vShadowCoord = uShadowProj * uShadowView * lPosition;

   // Calculate the relative normal
   vec4 lNormal = vec4(aNormal.xyz, 0.0);
   lNormal = uNormalMatrix * lNormal;
   vNormal = lNormal.xyz;

   // Texturing
   vTexCoord = aTexCoord;
}