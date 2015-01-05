#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class GameObject;
class NullCameraComponent;

class CameraComponent : public Component<CameraComponent, NullCameraComponent> {
public:
   virtual ~CameraComponent() {}

   virtual void tick(GameObject &gameObject, const float dt) = 0;

   virtual glm::vec3 getFrontVector(GameObject &gameObject) const = 0;

   virtual glm::vec3 getRightVector(GameObject &gameObject) const = 0;

   virtual glm::mat4 getViewMatrix(GameObject &gameObject) const = 0;

   virtual const glm::vec3& getCameraPosition(GameObject &gameObject) const = 0;
};

class NullCameraComponent : public CameraComponent {
private:
   const glm::vec3 DEFAULT_VEC3;
   const glm::mat4 DEFAULT_MAT4;

public:
   virtual ~NullCameraComponent() {}

   virtual void tick(GameObject &gameObject, const float dt) {}

   virtual glm::vec3 getFrontVector(GameObject &gameObject) const {
      return DEFAULT_VEC3;
   }

   virtual glm::vec3 getRightVector(GameObject &gameObject) const {
      return DEFAULT_VEC3;
   }

   virtual glm::mat4 getViewMatrix(GameObject &gameObject) const {
      return DEFAULT_MAT4;
   }

   virtual const glm::vec3& getCameraPosition(GameObject &gameObject) const {
      return DEFAULT_VEC3;
   }
};

#endif
