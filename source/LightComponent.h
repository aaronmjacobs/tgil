#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class ShaderProgram;
class ShadowMap;

class LightComponent : public Component {
public:
   enum LightType {
      Point = 0,
      Directional = 1,
      Spot = 2
   };

protected:
   LightType type;
   glm::vec3 color;
   glm::vec3 direction;
   float linearFalloff;
   float squareFalloff;
   float beamAngle;
   float cutoffAngle;
   SPtr<ShadowMap> shadowMap;

public:
   static const int MAX_LIGHTS = 10;

   LightComponent(GameObject &gameObject, LightType type = Point, const glm::vec3 &color = glm::vec3(0.0f), const glm::vec3 &direction = glm::vec3(0.0f), float linearFalloff = 0.0f, float squareFalloff = 0.0f, float beamAngle = 0.4f, float cutoffAngle = 0.5f);

   virtual ~LightComponent();

   virtual void draw(ShaderProgram &shaderProgram, const unsigned int index, int &shadowIndex, int &cubeShadowIndex);

   LightType getLightType() const {
      return type;
   }

   glm::mat4 getViewMatrix(int face) const;

   glm::mat4 getProjectionMatrix() const;

   glm::mat4 getBiasedProjectionMatrix() const;

   const glm::vec3& getDirection() const {
      return direction;
   }

   float getNearPlaneDist() const;

   float getFarPlaneDist() const;

   float getSquareFalloff() const {
      return squareFalloff;
   }

   void setSquareFalloff(float falloff) {
      squareFalloff = falloff;
   }

   void setDirection(const glm::vec3 &direction) {
      this->direction = direction;
   }

   SPtr<ShadowMap> getShadowMap() const {
      return shadowMap;
   }

   void setShadowMap(SPtr<ShadowMap> shadowMap) {
      this->shadowMap = shadowMap;
   }
};

#endif
