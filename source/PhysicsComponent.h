#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"

class btRigidBody;
class GameObject;
class NullPhysicsComponent;
class PhysicsManager;

class PhysicsComponent : public Component<PhysicsComponent, NullPhysicsComponent> {
protected:
   btRigidBody *rigidBody;

public:
   PhysicsComponent(SPtr<GameObject> gameObject, float mass)
      : rigidBody(nullptr) {}

   virtual ~PhysicsComponent() {}

   virtual void tick(GameObject &gameObject) = 0;

   btRigidBody* getRigidBody() {
      return rigidBody;
   }
};

class NullPhysicsComponent : public PhysicsComponent {
public:
   NullPhysicsComponent()
      : PhysicsComponent(nullptr, 0.0f) {}

   virtual ~NullPhysicsComponent() {}

   virtual void tick(GameObject &gameObject) {}
};

#endif
