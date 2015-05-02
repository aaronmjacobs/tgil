#ifndef MENU_LOGIC_COMPONENT_H
#define MENU_LOGIC_COMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "LogicComponent.h"
#include "Scene.h"

class btCollisionObject;

class MenuLogicComponent : public LogicComponent {
protected:
   float positionInterpTime, orientationInterpTime;
   glm::vec3 targetPosition, lastPosition;
   glm::quat targetOrientation, lastOrientation;

   const btCollisionObject *lastObject;
   bool wasClicking;

   bool updatePosition(const float dt);

   bool updateOrientation(const float dt);

   const btCollisionObject* getHitObject(InputHandler &inputHandler, const Scene &scene) const;

   void handleEvents(const Scene &scene, const btCollisionObject *hitObject, bool click);

public:
   MenuLogicComponent(GameObject &gameObject);

   virtual ~MenuLogicComponent();

   virtual void tick(const float dt);

   void setTargetPosition(const glm::vec3 &position);

   void setTargetOrientation(const glm::quat &orientation);
};


#endif
