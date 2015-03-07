#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class ShaderProgram;

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

public:
   LightComponent(GameObject &gameObject, LightType type = Point, const glm::vec3 &color = glm::vec3(0.0f), const glm::vec3 &direction = glm::vec3(0.0f), float linearFalloff = 0.0f, float squareFalloff = 0.0f, float beamAngle = 0.4f, float cutoffAngle = 0.5f);

   virtual ~LightComponent();

   virtual void draw(const ShaderProgram &shaderProgram, const unsigned int index);
};

#endif
