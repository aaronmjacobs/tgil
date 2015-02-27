#ifndef PLAYER_CAMERA_COMPONENT_H
#define PLAYER_CAMERA_COMPONENT_H

#include "CameraComponent.h"

class PlayerCameraComponent : public CameraComponent {
public:
   PlayerCameraComponent(GameObject &gameObject);

   virtual ~PlayerCameraComponent();

   virtual glm::vec3 getFrontVector() const;

   virtual glm::vec3 getRightVector() const;

   virtual glm::vec3 getUpVector() const;

   virtual glm::mat4 getViewMatrix() const;

   virtual glm::vec3 getCameraPosition() const;
};


#endif
