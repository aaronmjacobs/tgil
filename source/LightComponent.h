#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class GameObject;
class ShaderProgram;

class LightComponent : public Component {
protected:
   glm::vec3 color;

public:
   virtual ~LightComponent() {}

   virtual void draw(GameObject &gameObject, const ShaderProgram &shaderProgram, const unsigned int index) = 0;

   const glm::vec3& getColor() const {
      return color;
   }

   void setColor(const glm::vec3 &color) {
      this->color = color;
   }
};

#endif
