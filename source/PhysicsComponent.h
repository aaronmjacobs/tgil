#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"
#include "Observer.h"

#define BIT(x) (1<<(x))

class btCollisionObject;
class btCollisionShape;
class PhysicsManager;

namespace CollisionGroup {

// Represented internally in Bullet as a short, so we have 16 groups
enum Group {
   Nothing = 0,
   StaticBodies = BIT(0),
   DynamicBodies = BIT(1),
   Triggers = BIT(2),
   Players = BIT(3),
   Everything = -1
};

} // namsepace CollisionGroup

// TODO Handle CollisionTypes and CollisionGroups
class PhysicsComponent : public Component, public Observer<GameObject>, public std::enable_shared_from_this<PhysicsComponent> {
protected:
   const int collisionType;
   const CollisionGroup::Group collisionGroup;
   const CollisionGroup::Group collisionMask;
   UPtr<btCollisionObject> collisionObject;
   UPtr<btCollisionShape> collisionShape;
   WPtr<PhysicsManager> physicsManager;

public:
   PhysicsComponent(GameObject &gameObject, int collisionType, const CollisionGroup::Group collisionGroup, const CollisionGroup::Group collisionMask);

   virtual ~PhysicsComponent();

   virtual void init();

   const CollisionGroup::Group getCollisionGroup() const {
      return collisionGroup;
   }

   const CollisionGroup::Group getCollisionMask() const {
      return collisionMask;
   }

   btCollisionObject* getCollisionObject() const {
      return collisionObject.get();
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
