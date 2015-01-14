#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"
#include "Observer.h"

class btCollisionObject;
class btCollisionShape;
class PhysicsManager;

namespace CollisionType {

enum Type {
   Static,
   Dynamic,
   Kinematic
};

} // namespace CollisionType

class PhysicsComponent : public Component, public Observer<GameObject>, public std::enable_shared_from_this<PhysicsComponent> {
protected:
   const CollisionType::Type collisionType;
   UPtr<btCollisionObject> collisionObject;
   UPtr<btCollisionShape> collisionShape;
   WPtr<PhysicsManager> physicsManager;

public:
   PhysicsComponent(GameObject &gameObject, const CollisionType::Type collisionType)
      : Component(gameObject), collisionType(collisionType) {}

   virtual ~PhysicsComponent();

   virtual void init();

   btCollisionObject& getCollisionObject() const {
      return *collisionObject;
   }

   virtual void onNotify(const GameObject &gameObject, Event event);
};

class NullPhysicsComponent : public PhysicsComponent {
public:
   NullPhysicsComponent(GameObject &gameObject);

   virtual ~NullPhysicsComponent() {}

   virtual void init() {}

   virtual void onNotify(const GameObject &gameObject, Event event) {}
};

#endif
