#ifndef PHONG_MATERIAL_H
#define PHONG_MATERIAL_H

#include "Material.h"

#include <glm/glm.hpp>

class ShaderProgram;

class PhongMaterial : public Material {
protected:
   const glm::vec3 ambient, diffuse, specular, emission;
   const float shininess;

public:
   PhongMaterial(const glm::vec3 &ambient,
                 const glm::vec3 &diffuse,
                 const glm::vec3 &specular,
                 const glm::vec3 &emission,
                 const float shininess);

   virtual ~PhongMaterial();

   virtual void apply(ShaderProgram &shaderProgram);

   virtual void disable();
};

#endif
