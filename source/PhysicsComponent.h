#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"

class btRigidBody;
class NullPhysicsComponent;
class PhysicsManager;

class PhysicsComponent : public Component<PhysicsComponent, NullPhysicsComponent> {
protected:
   btRigidBody *rigidBody;

public:
   PhysicsComponent(GameObject &gameObject, float mass)
      : Component(gameObject), rigidBody(nullptr) {}

   virtual ~PhysicsComponent() {}

   virtual void tick() = 0;

   btRigidBody* getRigidBody() {
      return rigidBody;
   }
};

class NullPhysicsComponent : public PhysicsComponent {
public:
   NullPhysicsComponent()
      : PhysicsComponent(PhysicsComponent::getNullGameObject(), 0.0f) {}

   virtual ~NullPhysicsComponent() {}

   virtual void tick() {}
};

#endif
