#ifndef FLY_CAMERA_COMPONENT
#define FLY_CAMERA_COMPONENT

#include "CameraComponent.h"

class FlyCameraComponent : public CameraComponent {
public:
   FlyCameraComponent(GameObject &gameObject);

   virtual ~FlyCameraComponent();

   virtual glm::vec3 getFrontVector() const;

   virtual glm::vec3 getRightVector() const;

   virtual glm::vec3 getUpVector() const;

   virtual glm::mat4 getViewMatrix() const;

   virtual glm::vec3 getCameraPosition() const;
};

#endif
