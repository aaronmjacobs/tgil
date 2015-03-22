#include "GLIncludes.h"
#include "PhongMaterial.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <string>

PhongMaterial::PhongMaterial(const glm::vec3 &ambient,
              const glm::vec3 &diffuse,
              const glm::vec3 &specular,
              const glm::vec3 &emission,
              const float shininess)
   : ambient(ambient), diffuse(diffuse), specular(specular), emission(emission), shininess(shininess) {
}

PhongMaterial::~PhongMaterial() {
}

void PhongMaterial::apply(ShaderProgram &shaderProgram) {
   shaderProgram.setUniformValue("uMaterial.ambient", ambient);
   shaderProgram.setUniformValue("uMaterial.diffuse", diffuse);
   shaderProgram.setUniformValue("uMaterial.specular", specular);
   shaderProgram.setUniformValue("uMaterial.emission", emission);
   shaderProgram.setUniformValue("uMaterial.shininess", shininess);
}

void PhongMaterial::disable() {
}
