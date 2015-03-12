#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "Material.h"
#include "Model.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <string>

GeometricGraphicsComponent::GeometricGraphicsComponent(GameObject &gameObject)
   : GraphicsComponent(gameObject) {
}

GeometricGraphicsComponent::~GeometricGraphicsComponent() {
}

void GeometricGraphicsComponent::draw(const RenderData &renderData) {
   if (!model) {
      return;
   }

   SPtr<ShaderProgram> shaderProgram = model->getShaderProgram();
   shaderProgram->use();

   if (shaderProgram->hasUniform("uModelMatrix")) {
      const glm::mat4 &transMatrix = glm::translate(gameObject.getPosition());
      const glm::mat4 &rotMatrix = glm::toMat4(gameObject.getOrientation());
      const glm::mat4 &scaleMatrix = glm::scale(gameObject.getScale());
      const glm::mat4 &modelMatrix = transMatrix * rotMatrix * scaleMatrix;

      glUniformMatrix4fv(shaderProgram->getUniform("uModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

      if (shaderProgram->hasUniform("uNormalMatrix")) {
         const glm::mat4 &normalMatrix = glm::transpose(glm::inverse(modelMatrix));
         glUniformMatrix4fv(shaderProgram->getUniform("uNormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
      }
   }

   model->draw(renderData);
}
