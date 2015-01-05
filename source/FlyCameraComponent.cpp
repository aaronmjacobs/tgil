#include "FlyCameraComponent.h"
#include "GameObject.h"
#include "InputComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {

const float CAMERA_SPEED = 5.0f;

} // namespace

FlyCameraComponent::~FlyCameraComponent() {
}

void FlyCameraComponent::tick(GameObject &gameObject, const float dt) {
   InputValues inputValues = gameObject.getInputComponent().getInputValues(gameObject);

   float amount = dt * CAMERA_SPEED;
   float pitchAmount = amount * inputValues.lookY;
   float yawAmount = amount * inputValues.lookX;
   glm::vec3 front = getFrontVector(gameObject);
   glm::vec3 right = getRightVector(gameObject);

   const float Y_LOOK_BOUND = 0.99f;
   if (front.y - pitchAmount > Y_LOOK_BOUND) {
      pitchAmount = front.y - Y_LOOK_BOUND;
   }

   if (front.y - pitchAmount < -Y_LOOK_BOUND) {
      pitchAmount = front.y + Y_LOOK_BOUND;
   }

   glm::quat pitchChange = glm::angleAxis(pitchAmount, glm::vec3(1.0f, 0.0f, 0.0f));
   glm::quat yawChange = glm::angleAxis(yawAmount, glm::vec3(0.0f, 1.0f, 0.0f));

   glm::vec3 posChange = amount * (((inputValues.moveForward - inputValues.moveBackward) * front) + ((inputValues.moveRight - inputValues.moveLeft) * right));
   gameObject.setPosition(gameObject.getPosition() + posChange);
   gameObject.setOrientation(glm::normalize(pitchChange * gameObject.getOrientation() * yawChange));
}

glm::vec3 FlyCameraComponent::getFrontVector(GameObject &gameObject) const {
   return glm::vec3(0.0f, 0.0f, -1.0f) * glm::normalize(gameObject.getOrientation());
}

glm::vec3 FlyCameraComponent::getRightVector(GameObject &gameObject) const {
   return glm::vec3(1.0f, 0.0f, 0.0f) * glm::normalize(gameObject.getOrientation());
}

glm::mat4 FlyCameraComponent::getViewMatrix(GameObject &gameObject) const {
   const glm::vec3 position = getCameraPosition(gameObject);
   return glm::lookAt(position,
                      position + getFrontVector(gameObject),
                      glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector
}

const glm::vec3& FlyCameraComponent::getCameraPosition(GameObject &gameObject) const {
   return gameObject.getPosition();
}
