#include "GameObject.h"
#include "PointLightComponent.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>

PointLightComponent::~PointLightComponent() {
}

void PointLightComponent::draw(GameObject &gameObject, const ShaderProgram &shaderProgram, const unsigned int index) {
   std::stringstream ss;
   ss << "uLights[" << index << "]";
   const std::string &lightName = ss.str();

   glUniform1i(shaderProgram.getUniform("uNumLights"), 1);
   glUniform3fv(shaderProgram.getUniform(lightName + ".color"), 1, glm::value_ptr(color));
   glUniform3fv(shaderProgram.getUniform(lightName + ".position"), 1, glm::value_ptr(gameObject.getPosition()));
   glUniform1f(shaderProgram.getUniform(lightName + ".constFalloff"), 0.01f);
   glUniform1f(shaderProgram.getUniform(lightName + ".linearFalloff"), 0.01f);
   glUniform1f(shaderProgram.getUniform(lightName + ".squareFalloff"), 0.001f);
}
