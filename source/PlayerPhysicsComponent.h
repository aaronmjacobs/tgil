#ifndef PLAYER_PHYSICS_COMPONENT_H
#define PLAYER_PHYSICS_COMPONENT_H

#include "Observer.h"
#include "PhysicsComponent.h"

class btCollisionShape;
class btGhostObject;
class btMotionState;

class PlayerPhysicsComponent : public PhysicsComponent {
protected:
   UPtr<btMotionState> motionState;
   SPtr<btGhostObject> ghostObject;
   UPtr<btCollisionShape> ghostCollisionShape;

public:
   PlayerPhysicsComponent(GameObject &gameObject, float mass);

   virtual ~PlayerPhysicsComponent();

   virtual void addToManager(SPtr<PhysicsManager> manager);

   virtual void removeFromManager(SPtr<PhysicsManager> manager);

   virtual void onNotify(const GameObject &gameObject, Event event);
};

#endif
