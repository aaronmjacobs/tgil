#ifndef MESH_PHYSICS_COMPONENT_H
#define MESH_PHYSICS_COMPONENT_H

#include "Observer.h"
#include "PhysicsComponent.h"

class btCollisionShape;
class btMotionState;

class MeshPhysicsComponent : public PhysicsComponent, public Observer<GameObject>, public std::enable_shared_from_this<MeshPhysicsComponent> {
protected:
   UPtr<btCollisionShape> collisionShape;
   UPtr<btMotionState> motionState;

public:
   MeshPhysicsComponent(GameObject &gameObject, float mass);

   virtual ~MeshPhysicsComponent();

   virtual void init(GameObject &gameObject);

   virtual void tick(GameObject &gameObject);

   virtual void onNotify(const GameObject &gameObject, Event event);
};

#endif
