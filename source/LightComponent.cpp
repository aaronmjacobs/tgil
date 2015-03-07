#include "GameObject.h"
#include "LightComponent.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <string>

LightComponent::LightComponent(GameObject &gameObject, LightType type, const glm::vec3 &color, const glm::vec3 &direction, float linearFalloff, float squareFalloff, float beamAngle, float cutoffAngle)
   : Component(gameObject), type(type), color(color), direction(direction), linearFalloff(linearFalloff), squareFalloff(squareFalloff), beamAngle(beamAngle), cutoffAngle(cutoffAngle) {
}

LightComponent::~LightComponent() {
}

void LightComponent::draw(const ShaderProgram &shaderProgram, const unsigned int index) {
   std::stringstream ss;
   ss << "uLights[" << index << "]";
   const std::string &lightName = ss.str();

   shaderProgram.use();
   glUniform1i(shaderProgram.getUniform(lightName + ".type"), type);
   glUniform3fv(shaderProgram.getUniform(lightName + ".color"), 1, glm::value_ptr(color));
   glUniform3fv(shaderProgram.getUniform(lightName + ".position"), 1, glm::value_ptr(gameObject.getPosition()));
   glUniform3fv(shaderProgram.getUniform(lightName + ".direction"), 1, glm::value_ptr(direction));
   glUniform1f(shaderProgram.getUniform(lightName + ".linearFalloff"), linearFalloff);
   glUniform1f(shaderProgram.getUniform(lightName + ".squareFalloff"), squareFalloff);
   glUniform1f(shaderProgram.getUniform(lightName + ".beamAngle"), beamAngle);
   glUniform1f(shaderProgram.getUniform(lightName + ".cutoffAngle"), cutoffAngle);
}
