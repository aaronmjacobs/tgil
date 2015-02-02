#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>

PhysicsComponent::PhysicsComponent(GameObject &gameObject, const CollisionGroup::Group collisionGroup, const short collisionMask)
   : Component(gameObject), collisionGroup(collisionGroup), collisionMask(collisionMask) {
}

PhysicsComponent::~PhysicsComponent() {
   // Remove from the physics managers (avoiding concurrent modification)
   while (!physicsManagers.empty()) {
      WPtr<PhysicsManager> wManager = *physicsManagers.begin();
      SPtr<PhysicsManager> manager = wManager.lock();
      if (manager) {
         removeFromManager(manager);
      }
   }
}

void PhysicsComponent::init() {
   ASSERT(collisionObject, "Collision object not instantiated by physics component");
   ASSERT(collisionShape, "Collision shape not instantiated by physics component");

   collisionObject->setUserPointer(&gameObject);

   // Listen for events from the game object
   gameObject.addObserver(shared_from_this());
}

void PhysicsComponent::addToManager(SPtr<PhysicsManager> manager) {
   ASSERT(manager, "Trying to add to null manager");
   ASSERT(physicsManagers.count(manager) == 0, "Trying to add component to physics manager that it is already in");
   if (!collisionObject) {
      return;
   }

   btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(collisionObject.get());
   if (rigidBody) {
      manager->getDynamicsWorld().addRigidBody(rigidBody, collisionGroup, collisionMask);
   } else {
      manager->getDynamicsWorld().addCollisionObject(collisionObject.get(), collisionGroup, collisionMask);
   }

   physicsManagers.insert(manager);
}

void PhysicsComponent::removeFromManager(SPtr<PhysicsManager> manager) {
   ASSERT(manager, "Trying to remove from null manager");
   ASSERT(physicsManagers.count(manager) > 0, "Trying to remove component from physics manager that it is not in");
   if (!collisionObject) {
      return;
   }

   btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(collisionObject.get());
   if (rigidBody) {
      manager->getDynamicsWorld().removeRigidBody(rigidBody);
   } else {
      manager->getDynamicsWorld().removeCollisionObject(collisionObject.get());
   }

   physicsManagers.erase(manager);
}

void PhysicsComponent::onNotify(const GameObject &gameObject, Event event) {
   switch (event) {
      case SCALE: {
         collisionShape->setLocalScaling(toBt(gameObject.getScale()));
         break;
      }
      default:
         break;
   }
}

NullPhysicsComponent::NullPhysicsComponent(GameObject &gameObject)
   : PhysicsComponent(gameObject, CollisionGroup::Nothing, CollisionGroup::Nothing) {
}
