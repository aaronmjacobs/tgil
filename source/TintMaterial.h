#ifndef TINT_MATERIAL_H
#define TINT_MATERIAL_H

#include "Material.h"

#include <glm/glm.hpp>

class ShaderProgram;

class TintMaterial : public Material {
protected:
   float opacity;
   glm::vec3 tint;

public:
   TintMaterial(float opacity = 1.0f, const glm::vec3 &tint = glm::vec3(0.0f));

   virtual ~TintMaterial();

   virtual void apply(ShaderProgram &shaderProgram);

   virtual void disable();

   void setValues(float opacity, const glm::vec3 &tint);
};

#endif
