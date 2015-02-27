#include "GameObject.h"
#include "PlayerCameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace {

// TODO Calculate from physics object
const float HEAD_OFFSET = 0.7f;

} // namespace

PlayerCameraComponent::PlayerCameraComponent(GameObject &gameObject)
   : CameraComponent(gameObject) {
}

PlayerCameraComponent::~PlayerCameraComponent() {
}

glm::vec3 PlayerCameraComponent::getFrontVector() const {
   return glm::vec3(0.0f, 0.0f, -1.0f) * glm::normalize(gameObject.getOrientation());
}

glm::vec3 PlayerCameraComponent::getRightVector() const {
   return glm::vec3(1.0f, 0.0f, 0.0f) * glm::normalize(gameObject.getOrientation());
}

glm::vec3 PlayerCameraComponent::getUpVector() const {
   return glm::cross(getRightVector(), getFrontVector());
}

glm::mat4 PlayerCameraComponent::getViewMatrix() const {
   const glm::vec3 position = getCameraPosition();
   return glm::lookAt(position,
                      position + getFrontVector(),
                      glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector
}

glm::vec3 PlayerCameraComponent::getCameraPosition() const {
   return gameObject.getPosition() + glm::vec3(0.0f, HEAD_OFFSET, 0.0f);
}
