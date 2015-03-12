#include "GameObject.h"
#include "PlayerGraphicsComponent.h"
#include "Material.h"
#include "Model.h"
#include "RenderData.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <string>

PlayerGraphicsComponent::PlayerGraphicsComponent(GameObject &gameObject)
   : GraphicsComponent(gameObject) {
}

PlayerGraphicsComponent::~PlayerGraphicsComponent() {
}

void PlayerGraphicsComponent::draw(const RenderData &renderData) {
   if (!model) {
      return;
   }

   const glm::mat4 &transMatrix = glm::translate(gameObject.getPosition());

   // Eliminate x and z rotations, flip y (to account for perspective)
   glm::quat rot = gameObject.getOrientation();
   rot.x = 0.0f;
   rot.y *= -1.0f;
   rot.z = 0.0f;
   rot = glm::normalize(rot);
   const glm::mat4 &rotMatrix = glm::toMat4(rot);

   const glm::mat4 &scaleMatrix = glm::scale(gameObject.getScale());
   const glm::mat4 &modelMatrix = transMatrix * rotMatrix * scaleMatrix;
   const glm::mat4 &normalMatrix = glm::transpose(glm::inverse(modelMatrix));

   SPtr<ShaderProgram> overrideProgram = renderData.getOverrideProgram();
   SPtr<ShaderProgram> shaderProgram = overrideProgram ? overrideProgram : model->getShaderProgram();
   shaderProgram->use();

   glUniformMatrix4fv(shaderProgram->getUniform("uModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
   glUniformMatrix4fv(shaderProgram->getUniform("uNormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

   model->draw(renderData);
}
