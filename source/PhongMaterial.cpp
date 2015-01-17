#include "PhongMaterial.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <string>

PhongMaterial::PhongMaterial(const ShaderProgram &shaderProgram,
              const glm::vec3 &ambient,
              const glm::vec3 &diffuse,
              const glm::vec3 &specular,
              const glm::vec3 &emission,
              const float shininess)
   : ambient(ambient), diffuse(diffuse), specular(specular), emission(emission), shininess(shininess) {
   uAmbient = shaderProgram.getUniform("uMaterial.ambient");
   uDiffuse = shaderProgram.getUniform("uMaterial.diffuse");
   uSpecular = shaderProgram.getUniform("uMaterial.specular");
   uEmission = shaderProgram.getUniform("uMaterial.emission");
   uShininess = shaderProgram.getUniform("uMaterial.shininess");
}

PhongMaterial::~PhongMaterial() {
}

void PhongMaterial::apply(const Mesh &mesh) {
   glUniform3fv(uAmbient, 1, glm::value_ptr(ambient));
   glUniform3fv(uDiffuse, 1, glm::value_ptr(diffuse));
   glUniform3fv(uSpecular, 1, glm::value_ptr(specular));
   glUniform3fv(uEmission, 1, glm::value_ptr(emission));
   glUniform1f(uShininess, shininess);
}

void PhongMaterial::disable() {
}
