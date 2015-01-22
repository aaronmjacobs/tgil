#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>

PhysicsComponent::PhysicsComponent(GameObject &gameObject, int collisionType, const CollisionGroup::Group collisionGroup, const CollisionGroup::Group collisionMask)
   : Component(gameObject), collisionType(collisionType), collisionGroup(collisionGroup), collisionMask(collisionMask) {
}

PhysicsComponent::~PhysicsComponent() {
   // Remove the rigid body from the physics manager
   SPtr<PhysicsManager> currentPhysicsManager = physicsManager.lock();
   if (currentPhysicsManager) {
      currentPhysicsManager->removeObject(*this);
   }
}

void PhysicsComponent::init() {
   ASSERT(collisionObject, "Collision object not instantiated by physics component");
   ASSERT(collisionShape, "Collision shape not instantiated by physics component");

   collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | collisionType);
   collisionObject->setUserPointer(&gameObject);

   // Listen for events from the game object
   gameObject.addObserver(shared_from_this());
}

void PhysicsComponent::onNotify(const GameObject &gameObject, Event event) {
   switch (event) {
      case SET_SCENE: {
         SPtr<PhysicsManager> currentPhysicsManager = physicsManager.lock();
         if (currentPhysicsManager) {
            currentPhysicsManager->removeObject(*this);
         }

         SPtr<Scene> scene = gameObject.getScene().lock();
         physicsManager = scene ? scene->getPhysicsManager() : SPtr<PhysicsManager>();
         break;
      }
      case SCALE: {
         collisionShape->setLocalScaling(toBt(gameObject.getScale()));
         break;
      }
      default:
         break;
   }
}

NullPhysicsComponent::NullPhysicsComponent(GameObject &gameObject)
   : PhysicsComponent(gameObject, btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE, CollisionGroup::Nothing, CollisionGroup::Nothing) {
   collisionShape = UPtr<btCollisionShape>(new btSphereShape(0.0f));
   collisionObject = UPtr<btCollisionObject>(new btCollisionObject);
   collisionObject->setCollisionShape(collisionShape.get());

   // TODO Crash happens when destructing dynamics world, because null physics components don't currently have a broadphase proxy
}
