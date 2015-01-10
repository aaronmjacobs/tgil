#ifndef POINT_LIGHT_COMPONENT_H
#define POINT_LIGHT_COMPONENT_H

#include "LightComponent.h"

class PointLightComponent : public LightComponent {
protected:
   float constFalloff;
   float linearFalloff;
   float squareFalloff;

public:
   PointLightComponent(GameObject &gameObject);

   virtual ~PointLightComponent();

   virtual void draw(const ShaderProgram &shaderProgram, const unsigned int index);

   float getConstFalloff() const {
      return constFalloff;
   }

   float getLinearFalloff() const {
      return linearFalloff;
   }

   float getSquareFalloff() const {
      return squareFalloff;
   }

   void setConstFalloff(float constFalloff) {
      this->constFalloff = constFalloff;
   }

   void setLinearFalloff(float linearFalloff) {
      this->linearFalloff = linearFalloff;
   }

   void setSquareFalloff(float squareFalloff) {
      this->squareFalloff = squareFalloff;
   }
};

#endif
