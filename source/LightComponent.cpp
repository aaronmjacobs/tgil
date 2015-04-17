#include "FancyAssert.h"
#include "GameObject.h"
#include "LightComponent.h"
#include "ShaderProgram.h"
#include "ShadowMap.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <string>

// Yup, defined in windows.h for whatever reason
#ifdef _WIN32
#undef near
#undef far
#endif // _WIN32

LightComponent::LightComponent(GameObject &gameObject, LightType type, const glm::vec3 &color, const glm::vec3 &direction, float linearFalloff, float squareFalloff, float beamAngle, float cutoffAngle)
   : Component(gameObject), type(type), color(color), direction(direction), linearFalloff(linearFalloff), squareFalloff(squareFalloff), beamAngle(beamAngle), cutoffAngle(cutoffAngle) {
}

LightComponent::~LightComponent() {
}

void LightComponent::draw(ShaderProgram &shaderProgram, const unsigned int index, int &shadowIndex, int &cubeShadowIndex) {
   std::stringstream ss;
   ss << "uLights[" << index << "]";
   const std::string &lightName = ss.str();

   shaderProgram.setUniformValue(lightName + ".type", type);
   shaderProgram.setUniformValue(lightName + ".color", color);
   shaderProgram.setUniformValue(lightName + ".position", gameObject.getPosition());
   shaderProgram.setUniformValue(lightName + ".direction", direction);
   shaderProgram.setUniformValue(lightName + ".linearFalloff", linearFalloff);
   shaderProgram.setUniformValue(lightName + ".squareFalloff", squareFalloff);
   shaderProgram.setUniformValue(lightName + ".beamAngle", beamAngle);
   shaderProgram.setUniformValue(lightName + ".cutoffAngle", cutoffAngle);

   if (shadowMap) {
      // Shadow index
      int usedShadowIndex = -1;
      int usedCubeShadowIndex = -1;

      if (type == Point) {
         usedCubeShadowIndex = cubeShadowIndex++;
      } else {
         usedShadowIndex = shadowIndex++;
      }

      // Shadow weights
      for (int i = 0; i < 5; ++i) {
         ss.str(std::string());
         ss.clear();
         ss << ".shadowWeight" << i;

         shaderProgram.setUniformValue(lightName + ss.str(), i == usedShadowIndex ? 1.0f : 0.0f);
      }
      for (int i = 0; i < 4; ++i) {
         ss.str(std::string());
         ss.clear();
         ss << ".cubeShadowWeight" << i;

         shaderProgram.setUniformValue(lightName + ss.str(), i == usedCubeShadowIndex ? 1.0f : 0.0f);
      }

      // Shadow
      ss.str(std::string());
      ss.clear();
      if (usedShadowIndex != -1) {
         ss << "uShadows[" << usedShadowIndex << "]";
      } else {
         ss << "uCubeShadows[" << usedCubeShadowIndex << "]";
      }
      const std::string &shadowName = ss.str();

      if (shaderProgram.hasUniform(shadowName + ".near")) {
         shaderProgram.setUniformValue(shadowName + ".near", getNearPlaneDist());
         shaderProgram.setUniformValue(shadowName + ".far", getFarPlaneDist());
      }

      if (shaderProgram.hasUniform(shadowName + ".shadowProj")) {
         shaderProgram.setUniformValue(shadowName + ".shadowProj", getBiasedProjectionMatrix());
         shaderProgram.setUniformValue(shadowName + ".shadowView", getViewMatrix(-1));
      }

      GLenum shadowTextureUnit = shadowMap->bindTexture();;
      shaderProgram.setUniformValue(shadowName + ".shadowMap", shadowTextureUnit);
   }
}

glm::mat4 LightComponent::getViewMatrix(int face) const {
   ASSERT(face >= 0 && face < 6 || face == -1, "Invalid face value");

   // TODO Determine by scene geometry?
   const float directionalLightDist = 50.0f;

   const glm::vec3 &pos = gameObject.getPosition();
   glm::vec3 look;
   glm::vec3 up(0.0f, 1.0f, 0.0f);
   switch (face) {
      case 0:
         look = glm::vec3(1.0f, 0.0f, 0.0f);
         up = glm::vec3(0.0f, -1.0f, 0.0f);
         break;
      case 1:
         look = glm::vec3(-1.0f, 0.0f, 0.0f);
         up = glm::vec3(0.0f, -1.0f, 0.0f);
         break;
      case 2:
         look = glm::vec3(0.0f, 1.0f, 0.0f);
         up = glm::vec3(0.0f, 0.0f, 1.0f);
         break;
      case 3:
         look = glm::vec3(0.0f, -1.0f, 0.0f);
         up = glm::vec3(0.0f, 0.0f, -1.0f);
         break;
      case 4:
         look = glm::vec3(0.0f, 0.0f, 1.0f);
         up = glm::vec3(0.0f, -1.0f, 0.0f);
         break;
      case 5:
         look = glm::vec3(0.0f, 0.0f, -1.0f);
         up = glm::vec3(0.0f, -1.0f, 0.0f);
         break;
   }

   switch (type) {
      case Point:
         return glm::lookAt(pos, pos + look, up);
      case Directional:
         return glm::lookAt(glm::normalize(-direction) * directionalLightDist, glm::vec3(0.0f), up);
      case Spot:
         return glm::lookAt(pos, pos + direction, up);
   }
}

glm::mat4 LightComponent::getProjectionMatrix() const {
   // TODO Determine by scene geometry?
   const float directionWidth = 100.0f;

   float near = getNearPlaneDist();
   float far = getFarPlaneDist();

   switch (type) {
      case Point:
         return glm::perspective(glm::radians(90.0f), 1.0f, near, far);
      case Directional:
         return glm::ortho<float>(-directionWidth, directionWidth, -directionWidth, directionWidth, near, far);
      case Spot:
         return glm::perspective(cutoffAngle * 2.0f, 1.0f, near, far);
   }
}

glm::mat4 LightComponent::getBiasedProjectionMatrix() const {
   const glm::mat4 bias = {
      0.5f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.5f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.5f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f };

   return bias * getProjectionMatrix();
}

float LightComponent::getNearPlaneDist() const {
   if (type == Directional) {
      return 0.0f;
   }

   return 0.25f;
}

float LightComponent::getFarPlaneDist() const {
   const float lightCutoffAttenuation = 0.01f;
   float cutoffDist = 100.0f;

   if (squareFalloff * lightCutoffAttenuation > 0.0f) {
      cutoffDist = glm::sqrt(1.0f / (squareFalloff * lightCutoffAttenuation));
   }

   return cutoffDist;
}
