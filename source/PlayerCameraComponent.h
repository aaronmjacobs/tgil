#ifndef PLAYER_CAMERA_COMPONENT_H
#define PLAYER_CAMERA_COMPONENT_H

#include "CameraComponent.h"

class PlayerCameraComponent : public CameraComponent {
protected:
   bool wasJumpingLastFrame;

public:
   PlayerCameraComponent(GameObject &gameObject);

   virtual ~PlayerCameraComponent();

   virtual void tick(const float dt);

   virtual glm::vec3 getFrontVector() const;

   virtual glm::vec3 getRightVector() const;

   virtual glm::mat4 getViewMatrix() const;

   virtual glm::vec3 getCameraPosition() const;
};


#endif
