#ifndef MESH_PHYSICS_COMPONENT_H
#define MESH_PHYSICS_COMPONENT_H

#include "PhysicsComponent.h"

class btCollisionShape;
class btMotionState;

class MeshPhysicsComponent : public PhysicsComponent {
protected:
   btCollisionShape *collisionShape;
   btMotionState *motionState;

public:
   MeshPhysicsComponent(GameObject &gameObject, float mass);

   virtual ~MeshPhysicsComponent();

   virtual void tick();
};

#endif
