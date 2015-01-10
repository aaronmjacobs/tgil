#include "FlyCameraComponent.h"
#include "GameObject.h"
#include "InputComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {

const float CAMERA_SPEED = 5.0f;
const float Y_LOOK_BOUND = 0.99f;

} // namespace

FlyCameraComponent::FlyCameraComponent(GameObject &gameObject)
   : CameraComponent(gameObject) {
}

FlyCameraComponent::~FlyCameraComponent() {
}

void FlyCameraComponent::tick(const float dt) {
   const InputValues &inputValues = gameObject.getInputComponent().getInputValues();

   float amount = dt * CAMERA_SPEED;
   float pitchAmount = amount * inputValues.lookY;
   float yawAmount = amount * inputValues.lookX;
   glm::vec3 front = getFrontVector();
   glm::vec3 right = getRightVector();

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

glm::vec3 FlyCameraComponent::getFrontVector() const {
   return glm::vec3(0.0f, 0.0f, -1.0f) * glm::normalize(gameObject.getOrientation());
}

glm::vec3 FlyCameraComponent::getRightVector() const {
   return glm::vec3(1.0f, 0.0f, 0.0f) * glm::normalize(gameObject.getOrientation());
}

glm::mat4 FlyCameraComponent::getViewMatrix() const {
   const glm::vec3 position = getCameraPosition();
   return glm::lookAt(position,
                      position + getFrontVector(),
                      glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector
}

glm::vec3 FlyCameraComponent::getCameraPosition() const {
   return gameObject.getPosition();
}
