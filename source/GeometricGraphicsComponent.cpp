#include "GameObject.h"
#include "GeometricGraphicsComponent.h"
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

GeometricGraphicsComponent::GeometricGraphicsComponent(GameObject &gameObject)
   : GraphicsComponent(gameObject), castShadows(true) {
}

GeometricGraphicsComponent::~GeometricGraphicsComponent() {
}

void GeometricGraphicsComponent::draw(const RenderData &renderData) {
   if (!model) {
      return;
   }

   if (renderData.getRenderState() == RenderState::Shadow && !castShadows) {
      return;
   }

   SPtr<ShaderProgram> overrideProgram = renderData.getOverrideProgram();
   SPtr<ShaderProgram> shaderProgram = overrideProgram ? overrideProgram : model->getShaderProgram();

   if (shaderProgram->hasUniform("uModelMatrix")) {
      const glm::mat4 &transMatrix = glm::translate(gameObject.getPosition());
      const glm::mat4 &rotMatrix = glm::toMat4(gameObject.getOrientation());
      const glm::mat4 &scaleMatrix = glm::scale(gameObject.getScale());
      const glm::mat4 &modelMatrix = transMatrix * rotMatrix * scaleMatrix;
      shaderProgram->setUniformValue("uModelMatrix", modelMatrix);

      if (shaderProgram->hasUniform("uNormalMatrix")) {
         const glm::mat4 &normalMatrix = glm::transpose(glm::inverse(modelMatrix));
         shaderProgram->setUniformValue("uNormalMatrix", normalMatrix);
      }
   }

   model->draw(renderData);
}

void GeometricGraphicsComponent::enableCastingShadows(bool enabled) {
   castShadows = enabled;
}
