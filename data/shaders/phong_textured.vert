#version 330 core

#define SHADOWS

#define MAX_SHADOWS 5

#ifdef SHADOWS
struct Shadow {
   mat4 shadowView, shadowProj;
   sampler2DShadow shadowMap;
};
#endif

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

uniform int uNumLights;

#ifdef SHADOWS
uniform Shadow uShadows[MAX_SHADOWS];
#endif

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vWorldPosition;
out vec3 vNormal;
out vec2 vTexCoord;
#ifdef SHADOWS
out vec4 vShadowCoords[MAX_SHADOWS];
#endif

void main() {
   // Transforms
   vec4 lPosition = uModelMatrix * vec4(aPosition.xyz, 1.0);
   vWorldPosition = lPosition.xyz;
   gl_Position = uProjMatrix * uViewMatrix * lPosition;

#ifdef SHADOWS
   // Shadow coordinates
   vShadowCoords[0] = uShadows[0].shadowProj * uShadows[0].shadowView * lPosition;
   vShadowCoords[1] = uShadows[1].shadowProj * uShadows[1].shadowView * lPosition;
   vShadowCoords[2] = uShadows[2].shadowProj * uShadows[2].shadowView * lPosition;
   vShadowCoords[3] = uShadows[3].shadowProj * uShadows[3].shadowView * lPosition;
   vShadowCoords[4] = uShadows[4].shadowProj * uShadows[4].shadowView * lPosition;
#endif

   // Calculate the relative normal
   vec4 lNormal = vec4(aNormal.xyz, 0.0);
   lNormal = uNormalMatrix * lNormal;
   vNormal = lNormal.xyz;

   // Texturing
   vTexCoord = aTexCoord;
}