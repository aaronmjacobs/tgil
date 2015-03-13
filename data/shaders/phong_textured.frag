#version 330 core

#define MAX_LIGHTS 10
#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_DIRECTIONAL 1
#define LIGHT_TYPE_SPOT 2

struct Light {
  int type;
  vec3 color, position, direction;
  float linearFalloff, squareFalloff, beamAngle, cutoffAngle;
};

struct Material {
  vec3 ambient, diffuse, specular, emission;
  float shininess;
};

uniform Light uLights[MAX_LIGHTS];
uniform int uNumLights;
uniform Material uMaterial;
uniform vec3 uCameraPos;
uniform sampler2D uTexture;
uniform sampler2DShadow uShadowMap;

in vec3 vWorldPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vShadowCoord;

out vec4 color;

vec3 calcLighting(vec3 lNormal, vec3 surfaceColor, Light light) {
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
      float spotAngle = acos(dot(-light.direction, toLight));
      if (spotAngle >= light.cutoffAngle) {
         spotMultiplier = 0.0;
      } else if (spotAngle <= light.beamAngle) {
         spotMultiplier = 1.0;
      } else {
         spotMultiplier = (spotAngle - light.cutoffAngle) / (light.beamAngle - light.cutoffAngle);
      }
   }

   return (surfaceColor * diffuseAmount
          + uMaterial.specular * specularAmount)
          * falloff * spotMultiplier * light.color;
}

void main() {
   vec3 lNormal = normalize(vNormal);
   vec3 surfaceColor = texture(uTexture, vTexCoord).rgb;

   vec3 finalColor = vec3(0.0);
   for (int i = 0; i < uNumLights; ++i) {
      finalColor += calcLighting(lNormal, surfaceColor, uLights[i]);
   }

   // TODO Do for each light
   float bias = 0.001;
   vec3 shadowInfo = vec3(vShadowCoord.xy, vShadowCoord.z - bias);
   float visibility = texture(uShadowMap, shadowInfo);
   finalColor *= visibility * 0.75 + 0.25;

   finalColor += uMaterial.ambient;
   finalColor += uMaterial.emission;

   color = vec4(finalColor, 1.0);
}