#include "FlyCameraComponent.h"
#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>

FlyCameraComponent::FlyCameraComponent(GameObject &gameObject)
   : CameraComponent(gameObject) {
}

FlyCameraComponent::~FlyCameraComponent() {
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
