#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class ShaderProgram;

class LightComponent : public Component {
protected:
   glm::vec3 color;

public:
   LightComponent(GameObject &gameObject)
      : Component(gameObject) {}

   virtual ~LightComponent() {}

   virtual void draw(const ShaderProgram &shaderProgram, const unsigned int index) = 0;

   const glm::vec3& getColor() const {
      return color;
   }

   void setColor(const glm::vec3 &color) {
      this->color = color;
   }
};

class NullLightComponent : public LightComponent {
protected:
   const glm::vec3 COLOR;

public:
   NullLightComponent(GameObject &gameObject)
      : LightComponent(gameObject) {}

   virtual ~NullLightComponent() {}

   virtual void draw(const ShaderProgram &shaderProgram, const unsigned int index) {}

   const glm::vec3& getColor() const {
      return COLOR;
   }

   void setColor(const glm::vec3 &color) {}
};

#endif
