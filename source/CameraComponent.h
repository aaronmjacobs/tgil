#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class GameObject;

class CameraComponent : public Component {
public:
   virtual ~CameraComponent() {}

   virtual glm::vec3 getFrontVector(GameObject &gameObject) const = 0;

   virtual glm::mat4 getViewMatrix(GameObject &gameObject) const = 0;

   virtual const glm::vec3& getCameraPosition(GameObject &gameObject) const = 0;
};

#endif
