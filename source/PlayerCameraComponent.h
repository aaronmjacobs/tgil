#ifndef PLAYER_CAMERA_COMPONENT_H
#define PLAYER_CAMERA_COMPONENT_H

#include "CameraComponent.h"

class PlayerCameraComponent : public CameraComponent {
public:
   virtual ~PlayerCameraComponent();

   virtual void tick(GameObject &gameObject, const float dt);

   virtual glm::vec3 getFrontVector(GameObject &gameObject) const;

   virtual glm::vec3 getRightVector(GameObject &gameObject) const;

   virtual glm::mat4 getViewMatrix(GameObject &gameObject) const;

   virtual glm::vec3 getCameraPosition(GameObject &gameObject) const;
};


#endif
