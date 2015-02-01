#ifndef MESH_PHYSICS_COMPONENT_H
#define MESH_PHYSICS_COMPONENT_H

#include "PhysicsComponent.h"

class btCollisionShape;
class btMotionState;

// TODO Doesn't necessarily have to be a rigid body - different classes?
class MeshPhysicsComponent : public PhysicsComponent {
protected:
   UPtr<btMotionState> motionState;

public:
   MeshPhysicsComponent(GameObject &gameObject, float mass, const CollisionGroup::Group collisionGroup, const short collisionMask);

   virtual ~MeshPhysicsComponent();
};

#endif
