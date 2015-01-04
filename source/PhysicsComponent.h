#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"

class btRigidBody;
class GameObject;
class NullPhysicsComponent;
class PhysicsManager;

class PhysicsComponent : public Component<PhysicsComponent, NullPhysicsComponent> {
protected:
   UPtr<btRigidBody> rigidBody;
   WPtr<PhysicsManager> physicsManager;

public:
   virtual ~PhysicsComponent() {}

   virtual void tick(GameObject &gameObject) = 0;

   virtual void onAdd(SPtr<PhysicsManager> physicsManager) = 0;

   btRigidBody* getRigidBody() const {
      return rigidBody.get();
   }
};

class NullPhysicsComponent : public PhysicsComponent {
public:
   virtual ~NullPhysicsComponent() {}

   virtual void tick(GameObject &gameObject) {}

   virtual void onAdd(SPtr<PhysicsManager> physicsManager) {}
};

#endif
