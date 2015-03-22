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

void LightComponent::draw(ShaderProgram &shaderProgram, const unsigned int index) {
   std::stringstream ss;
   ss << "uLights[" << index << "]";
   const std::string &lightName = ss.str();

   shaderProgram.setUniformValue(lightName + ".type", type);
   shaderProgram.setUniformValue(lightName + ".color", color);
   shaderProgram.setUniformValue(lightName + ".position", gameObject.getPosition());
   shaderProgram.setUniformValue(lightName + ".direction", direction);
   shaderProgram.setUniformValue(lightName + ".linearFalloff", linearFalloff);
   shaderProgram.setUniformValue(lightName + ".squareFalloff", squareFalloff);
   shaderProgram.setUniformValue(lightName + ".beamAngle", beamAngle);
   shaderProgram.setUniformValue(lightName + ".cutoffAngle", cutoffAngle);
}
