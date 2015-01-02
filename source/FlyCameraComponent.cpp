#include "FlyCameraComponent.h"
#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>

FlyCameraComponent::FlyCameraComponent(GameObject& gameObject)
   : CameraComponent(gameObject) {
}

FlyCameraComponent::~FlyCameraComponent() {
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
