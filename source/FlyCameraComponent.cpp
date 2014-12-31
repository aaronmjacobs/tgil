#include "FlyCameraComponent.h"
#include "GameObject.h"

#include <glm/gtc/matrix_transform.hpp>

FlyCameraComponent::~FlyCameraComponent() {
}

glm::mat4 FlyCameraComponent::getViewMatrix(GameObject &gameObject) const {
   const glm::quat &orientation = glm::normalize(gameObject.getOrientation());
   glm::vec3 front = glm::normalize(glm::vec3(-2 * (orientation.x * orientation.z + orientation.w * orientation.y),
                                              -2 * (orientation.y * orientation.z - orientation.w * orientation.x),
                                              -1 + 2 * (orientation.x * orientation.x + orientation.y * orientation.y)));

   const glm::vec3 position = getCameraPosition(gameObject);
   return glm::lookAt(position,
                      position + front,
                      glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector
}

const glm::vec3& FlyCameraComponent::getCameraPosition(GameObject &gameObject) const {
   return gameObject.getPosition();
}
