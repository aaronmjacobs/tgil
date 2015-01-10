#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"
#include "Observer.h"

class btRigidBody;
class PhysicsManager;

class PhysicsComponent : public Component, public Observer<GameObject>, public std::enable_shared_from_this<PhysicsComponent> {
protected:
   UPtr<btRigidBody> rigidBody;
   WPtr<PhysicsManager> physicsManager;

public:
   PhysicsComponent(GameObject &gameObject)
      : Component(gameObject) {}

   virtual ~PhysicsComponent();

   virtual void init();

   virtual void tick() = 0;

   btRigidBody* getRigidBody() const {
      return rigidBody.get();
   }

   virtual void onNotify(const GameObject &gameObject, Event event);
};

class NullPhysicsComponent : public PhysicsComponent {
public:
   NullPhysicsComponent(GameObject &gameObject)
      : PhysicsComponent(gameObject) {}

   virtual ~NullPhysicsComponent() {}

   virtual void init() {}

   virtual void tick() {}

   virtual void onNotify(const GameObject &gameObject, Event event) {}
};

#endif
