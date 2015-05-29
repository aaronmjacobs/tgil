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
   : GraphicsComponent(gameObject), headOffset(0.0f), leftHandOffset(0.0f), rightHandOffset(0.0f), leftFootOffset(0.0f), rightFootOffset(0.0f) {
   normalOffsetShadows = false;
}

PlayerGraphicsComponent::~PlayerGraphicsComponent() {
}

void PlayerGraphicsComponent::drawAppendages(const RenderData &renderData, const glm::mat4 &rotMatrix, const glm::mat4 &scaleMatrix) {
   if (headModel && !renderData.isRenderingCameraObject()) {
      glm::quat vertRot = gameObject.getOrientation();
      vertRot.x *= -1.0f;
      vertRot.y = 0.0f;
      vertRot.z = 0.0f;
      vertRot = glm::normalize(vertRot);
      const glm::mat4 &vertRotMatrix = glm::toMat4(vertRot);
      drawAppendage(renderData, rotMatrix, vertRotMatrix, scaleMatrix, headModel, headOffset);
   }

   if (handModel) {
      glm::quat vertRot = gameObject.getOrientation();
      vertRot.x *= -1.0f;
      vertRot.y = 0.0f;
      vertRot.z = 0.0f;
      vertRot = glm::normalize(vertRot);
      vertRot /= 2.0f;
      const glm::mat4 &vertRotMatrix = glm::toMat4(vertRot);
      drawAppendage(renderData, rotMatrix, vertRotMatrix, scaleMatrix, handModel, leftHandOffset);
      drawAppendage(renderData, rotMatrix, vertRotMatrix, scaleMatrix, handModel, rightHandOffset);
   }

   if (footModel) {
      drawAppendage(renderData, rotMatrix, glm::mat4(1.0f), scaleMatrix, footModel, leftFootOffset);
      drawAppendage(renderData, rotMatrix, glm::mat4(1.0f), scaleMatrix, footModel, rightFootOffset);
   }
}

void PlayerGraphicsComponent::drawAppendage(const RenderData &renderData, const glm::mat4 &rotMatrix, const glm::mat4 &vertRotMatrix, const glm::mat4 &scaleMatrix, SPtr<Model> model, const glm::vec3 &offset) {
   SPtr<ShaderProgram> overrideProgram = renderData.getOverrideProgram();

   const glm::mat4 &offsetMatrix = glm::translate(offset);
   const glm::mat4 &transMatrix = glm::translate(gameObject.getPosition());
   const glm::mat4 &modelMatrix = transMatrix * offsetMatrix * rotMatrix * vertRotMatrix * scaleMatrix;
   const glm::mat4 &normalMatrix = glm::transpose(glm::inverse(modelMatrix));

   SPtr<ShaderProgram> shaderProgram = overrideProgram ? overrideProgram : model->getShaderProgram();

   shaderProgram->setUniformValue("uModelMatrix", modelMatrix, true);
   shaderProgram->setUniformValue("uNormalMatrix", normalMatrix, true);

   model->draw(renderData);
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

   shaderProgram->setUniformValue("uModelMatrix", modelMatrix, true);
   shaderProgram->setUniformValue("uNormalMatrix", normalMatrix, true);

   model->draw(renderData);

   drawAppendages(renderData, rotMatrix, scaleMatrix);
}
