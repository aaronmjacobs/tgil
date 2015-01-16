#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "Component.h"

#include <glm/glm.hpp>

class CameraComponent : public Component {
public:
   CameraComponent(GameObject &gameObject)
      : Component(gameObject) {}

   virtual ~CameraComponent() {}

   virtual glm::vec3 getFrontVector() const = 0;

   virtual glm::vec3 getRightVector() const = 0;

   virtual glm::mat4 getViewMatrix() const = 0;

   virtual glm::vec3 getCameraPosition() const = 0;
};

class NullCameraComponent : public CameraComponent {
private:
   const glm::vec3 DEFAULT_VEC3;
   const glm::mat4 DEFAULT_MAT4;

public:
   NullCameraComponent(GameObject &gameObject)
      : CameraComponent(gameObject) {}

   virtual ~NullCameraComponent() {}

   virtual glm::vec3 getFrontVector() const {
      return DEFAULT_VEC3;
   }

   virtual glm::vec3 getRightVector() const {
      return DEFAULT_VEC3;
   }

   virtual glm::mat4 getViewMatrix() const {
      return DEFAULT_MAT4;
   }

   virtual glm::vec3 getCameraPosition() const {
      return DEFAULT_VEC3;
   }
};

#endif
