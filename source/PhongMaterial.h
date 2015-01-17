#ifndef PHONG_MATERIAL_H
#define PHONG_MATERIAL_H

#include "GLIncludes.h"
#include "Material.h"

#include <glm/glm.hpp>

class ShaderProgram;

class PhongMaterial : public Material {
protected:
   const glm::vec3 ambient, diffuse, specular, emission;
   const float shininess;
   GLint uAmbient, uDiffuse, uSpecular, uEmission, uShininess;

public:
   PhongMaterial(const ShaderProgram &shaderProgram,
                 const glm::vec3 &ambient,
                 const glm::vec3 &diffuse,
                 const glm::vec3 &specular,
                 const glm::vec3 &emission,
                 const float shininess);

   virtual ~PhongMaterial();

   virtual void apply(const Mesh &mesh);

   virtual void disable();
};

#endif
