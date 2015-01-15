#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>

PhysicsComponent::~PhysicsComponent() {
   // Remove the rigid body from the physics manager
   SPtr<PhysicsManager> currentPhysicsManager = physicsManager.lock();
   if (currentPhysicsManager) {
      currentPhysicsManager->removeObject(*this);
   }
}

void PhysicsComponent::init() {
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
      default:
         break;
   }
}

NullPhysicsComponent::NullPhysicsComponent(GameObject &gameObject)
   : PhysicsComponent(gameObject, CollisionType::Static, CollisionGroup::Nothing, CollisionGroup::Nothing) {
   collisionShape = UPtr<btCollisionShape>(new btSphereShape(0.0f));
   collisionObject = UPtr<btCollisionObject>(new btCollisionObject);
   collisionObject->setCollisionShape(collisionShape.get());
}
