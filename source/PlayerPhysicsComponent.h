#ifndef PLAYER_PHYSICS_COMPONENT_H
#define PLAYER_PHYSICS_COMPONENT_H

#include "Observer.h"
#include "PhysicsComponent.h"

class btCollisionShape;
class btMotionState;

class PlayerPhysicsComponent : public PhysicsComponent {
protected:
   UPtr<btMotionState> motionState;

public:
   PlayerPhysicsComponent(GameObject &gameObject, float mass);

   virtual ~PlayerPhysicsComponent();
};

#endif
