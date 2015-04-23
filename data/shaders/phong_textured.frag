#version 330 core

#define SHADOWS

#define MAX_LIGHTS 10
#define MAX_SHADOWS 5
#define MAX_CUBE_SHADOWS 4

#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT 2

struct Light {
   int type;
   vec3 color, position, direction;
   float linearFalloff, squareFalloff, beamAngle, cutoffAngle;

#ifdef SHADOWS
   float shadowWeight0;
   float shadowWeight1;
   float shadowWeight2;
   float shadowWeight3;
   float shadowWeight4;

   float cubeShadowWeight0;
   float cubeShadowWeight1;
   float cubeShadowWeight2;
   float cubeShadowWeight3;
#endif
};

#ifdef SHADOWS
struct Shadow {
   mat4 shadowView, shadowProj;
   sampler2DShadow shadowMap;
};

struct CubeShadow {
   float near, far;
   samplerCubeShadow shadowMap;
};
#endif

struct Material {
  vec3 ambient, diffuse, specular, emission;
  float shininess;
};

uniform Light uLights[MAX_LIGHTS];
uniform int uNumLights;
uniform Material uMaterial;
uniform vec3 uCameraPos;
uniform sampler2D uTexture;

#ifdef SHADOWS
uniform Shadow uShadows[MAX_SHADOWS];
uniform CubeShadow uCubeShadows[MAX_CUBE_SHADOWS];
#endif

in vec3 vWorldPosition;
in vec3 vNormal;
in vec2 vTexCoord;
#ifdef SHADOWS
in vec4 vShadowCoords[MAX_SHADOWS];
#endif

out vec4 color;

#ifdef SHADOWS
float vectorToDepthValue(in vec3 vec, in float near, in float far) {
   vec3 absVec = abs(vec);
   float localZComp = max(absVec.x, max(absVec.y, absVec.z));

   float normZComp = (far + near) / (far - near) - ((2.0 * far * near) / (far - near)) / localZComp;
   return (normZComp + 1.0) * 0.5;
}

float calcVisibility(in Light light, in float nDotL) {
   const float baseBias = 0.001;
   const float maxBias = 0.01;
   float bias = baseBias * tan(acos(nDotL));
   bias = clamp(bias, 0.0, maxBias);

   vec3 fromLight = vWorldPosition - light.position;
   vec3 nFromLight = normalize(fromLight);

   float shadowVisibilities[MAX_SHADOWS];
   float cubeShadowVisibilities[MAX_CUBE_SHADOWS];

   shadowVisibilities[0] = textureProj(uShadows[0].shadowMap, vec4(vShadowCoords[0].xy, vShadowCoords[0].z - bias, vShadowCoords[0].w));
   shadowVisibilities[1] = textureProj(uShadows[1].shadowMap, vec4(vShadowCoords[1].xy, vShadowCoords[1].z - bias, vShadowCoords[1].w));
   shadowVisibilities[2] = textureProj(uShadows[2].shadowMap, vec4(vShadowCoords[2].xy, vShadowCoords[2].z - bias, vShadowCoords[2].w));
   shadowVisibilities[3] = textureProj(uShadows[3].shadowMap, vec4(vShadowCoords[3].xy, vShadowCoords[3].z - bias, vShadowCoords[3].w));
   shadowVisibilities[4] = textureProj(uShadows[4].shadowMap, vec4(vShadowCoords[4].xy, vShadowCoords[4].z - bias, vShadowCoords[4].w));

   cubeShadowVisibilities[0] = texture(uCubeShadows[0].shadowMap,
      vec4(nFromLight, vectorToDepthValue(fromLight, uCubeShadows[0].near, uCubeShadows[0].far) - bias));
   cubeShadowVisibilities[1] = texture(uCubeShadows[1].shadowMap,
      vec4(nFromLight, vectorToDepthValue(fromLight, uCubeShadows[1].near, uCubeShadows[1].far) - bias));
   cubeShadowVisibilities[2] = texture(uCubeShadows[2].shadowMap,
      vec4(nFromLight, vectorToDepthValue(fromLight, uCubeShadows[2].near, uCubeShadows[2].far) - bias));
   cubeShadowVisibilities[3] = texture(uCubeShadows[3].shadowMap,
      vec4(nFromLight, vectorToDepthValue(fromLight, uCubeShadows[3].near, uCubeShadows[3].far) - bias));

   return
      shadowVisibilities[0] * light.shadowWeight0 +
      shadowVisibilities[1] * light.shadowWeight1 +
      shadowVisibilities[2] * light.shadowWeight2 +
      shadowVisibilities[3] * light.shadowWeight3 +
      shadowVisibilities[4] * light.shadowWeight4 +
      cubeShadowVisibilities[0] * light.cubeShadowWeight0 +
      cubeShadowVisibilities[1] * light.cubeShadowWeight1 +
      cubeShadowVisibilities[2] * light.cubeShadowWeight2 +
      cubeShadowVisibilities[3] * light.cubeShadowWeight3;
}
#endif

vec3 calcLighting(in vec3 lNormal, in vec3 surfaceColor, in Light light) {
   // Diffuse
   vec3 toLight;
   if (light.type == LIGHT_TYPE_DIRECTIONAL) {
      toLight = -light.direction;
   } else {
      toLight = light.position - vWorldPosition;
   }
   float lightDistance = length(toLight);
   toLight = normalize(toLight);
   float nDotL = dot(lNormal, toLight);
   float diffuseAmount = max(nDotL, 0.0);

   // Specular
   float specularAmount = 0.0;
   if (nDotL > 0.0) {
      vec3 toCamera = normalize(uCameraPos - vWorldPosition);
      vec3 reflection = normalize(2.0 * nDotL * lNormal - toLight);
      specularAmount = pow(max(dot(toCamera, reflection), 0.0), uMaterial.shininess);
   }

   // Falloff
   float falloff = 1.0;
   if (light.type != LIGHT_TYPE_DIRECTIONAL) {
      falloff /= 1.0
               + light.linearFalloff * lightDistance
               + light.squareFalloff * lightDistance * lightDistance;
   }

   // Spot light multiplier
   float spotMultiplier = 1.0;
   if (light.type == LIGHT_TYPE_SPOT) {
      float spotAngle = acos(dot(normalize(-light.direction), toLight));
      if (spotAngle >= light.cutoffAngle) {
         spotMultiplier = 0.0;
      } else if (spotAngle <= light.beamAngle) {
         spotMultiplier = 1.0;
      } else {
         spotMultiplier = (spotAngle - light.cutoffAngle) / (light.beamAngle - light.cutoffAngle);
      }
   }

#ifdef SHADOWS
   float visibility = calcVisibility(light, nDotL);
#else
   float visibility = 1.0;
#endif

   return (surfaceColor * diffuseAmount
          + uMaterial.specular * specularAmount)
          * visibility * falloff * spotMultiplier * light.color;
}

void main() {
   vec3 lNormal = normalize(vNormal);
   vec3 surfaceColor = texture(uTexture, vTexCoord).rgb;

   vec3 finalColor = vec3(0.0);
   for (int i = 0; i < uNumLights; ++i) {
      finalColor += calcLighting(lNormal, surfaceColor, uLights[i]);
   }

   float ambientAmount = (dot(lNormal, vec3(0.0, 1.0, 0.0)) + 1.0) / 2.0;
   ambientAmount = ambientAmount / 2.0 + 0.5;
   finalColor += uMaterial.ambient * surfaceColor * ambientAmount;
   finalColor += uMaterial.emission;

   color = vec4(finalColor, 1.0);
}