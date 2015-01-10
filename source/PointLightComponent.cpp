#include "GameObject.h"
#include "PointLightComponent.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <sstream>

PointLightComponent::PointLightComponent(GameObject &gameObject)
   : LightComponent(gameObject) {
}

PointLightComponent::~PointLightComponent() {
}

void PointLightComponent::draw(const ShaderProgram &shaderProgram, const unsigned int index) {
   std::stringstream ss;
   ss << "uLights[" << index << "]";
   const std::string &lightName = ss.str();

   shaderProgram.use();
   glUniform3fv(shaderProgram.getUniform(lightName + ".color"), 1, glm::value_ptr(color));
   glUniform3fv(shaderProgram.getUniform(lightName + ".position"), 1, glm::value_ptr(gameObject.getPosition()));
   glUniform1f(shaderProgram.getUniform(lightName + ".constFalloff"), constFalloff);
   glUniform1f(shaderProgram.getUniform(lightName + ".linearFalloff"), linearFalloff);
   glUniform1f(shaderProgram.getUniform(lightName + ".squareFalloff"), squareFalloff);
}
