#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
#include "Material.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <string>

GeometricGraphicsComponent::~GeometricGraphicsComponent() {
}

void GeometricGraphicsComponent::draw(GameObject &gameObject) {
   const glm::mat4 &transMatrix = glm::translate(gameObject.getPosition());
   const glm::mat4 &rotMatrix = glm::toMat4(gameObject.getOrientation());
   const glm::mat4 &scaleMatrix = glm::scale(gameObject.getScale());
   const glm::mat4 &modelMatrix = transMatrix * rotMatrix * scaleMatrix;
   const glm::mat4 &normalMatrix = glm::transpose(glm::inverse(modelMatrix));

   SPtr<Model> model = gameObject.getModel();
   if (!model) {
      return;
   }

   SPtr<ShaderProgram> shaderProgram = model->getMaterial().getShaderProgram();
   shaderProgram->use();

   glUniformMatrix4fv(shaderProgram->getUniform("uModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
   glUniformMatrix4fv(shaderProgram->getUniform("uNormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

   model->draw();
}
