#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"
#include "Observer.h"

class btRigidBody;
class GameObject;
class NullPhysicsComponent;
class PhysicsManager;

class PhysicsComponent : public Component<PhysicsComponent, NullPhysicsComponent>, public Observer<GameObject>, public std::enable_shared_from_this<PhysicsComponent> {
protected:
   UPtr<btRigidBody> rigidBody;
   WPtr<PhysicsManager> physicsManager;

public:
   virtual ~PhysicsComponent();

   virtual void init(GameObject &gameObject);

   virtual void tick(GameObject &gameObject) = 0;

   btRigidBody* getRigidBody() const {
      return rigidBody.get();
   }

   virtual void onNotify(const GameObject &gameObject, Event event);
};

class NullPhysicsComponent : public PhysicsComponent {
public:
   virtual ~NullPhysicsComponent() {}

   virtual void init(GameObject &gameObject) {}

   virtual void tick(GameObject &gameObject) {}

   virtual void onNotify(const GameObject &gameObject, Event event) {}
};

#endif
