#ifndef FLY_CAMERA_COMPONENT
#define FLY_CAMERA_COMPONENT

#include "CameraComponent.h"

class FlyCameraComponent : public CameraComponent {
protected:
   glm::mat4 projectionMatrix;

public:
   virtual ~FlyCameraComponent();

   virtual void tick(GameObject &gameObject, const float dt);

   virtual glm::vec3 getFrontVector(GameObject &gameObject) const;

   virtual glm::vec3 getRightVector(GameObject &gameObject) const;

   virtual glm::mat4 getViewMatrix(GameObject &gameObject) const;

   virtual const glm::vec3& getCameraPosition(GameObject &gameObject) const;
};

#endif
