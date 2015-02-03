#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"
#include "Observer.h"

#include <set>

#define BIT(x) (1<<(x))

class btCollisionObject;
class btCollisionShape;
class PhysicsManager;

namespace CollisionGroup {

// Represented internally in Bullet as a short, so we have 16 groups
// Bits 0-5 reserved by bullet
enum Group : short {
   Nothing = 0,
   Default = BIT(0),
   StaticBodies = BIT(1),
   KinematicBodies = BIT(2),
   Debries = BIT(3),
   Sensors = BIT(4),
   Characters = BIT(5),
   Ghosts = BIT(6),
   Everything = -1
};

} // namsepace CollisionGroup

class PhysicsComponent : public Component, public Observer<GameObject>, public std::enable_shared_from_this<PhysicsComponent> {
protected:
   const CollisionGroup::Group collisionGroup;
   const short collisionMask;
   UPtr<btCollisionObject> collisionObject;
   UPtr<btCollisionShape> collisionShape;
   std::set<WPtr<PhysicsManager>, std::owner_less<WPtr<PhysicsManager>>> physicsManagers;

public:
   PhysicsComponent(GameObject &gameObject, const CollisionGroup::Group collisionGroup, const short collisionMask);

   virtual ~PhysicsComponent();

   virtual void init();

   const short getCollisionGroup() const {
      return collisionGroup;
   }

   const short getCollisionMask() const {
      return collisionMask;
   }

   btCollisionObject* getCollisionObject() const {
      return collisionObject.get();
   }

   virtual void addToManager(SPtr<PhysicsManager> manager);

   virtual void removeFromManager(SPtr<PhysicsManager> manager);

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
