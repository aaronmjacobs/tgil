#include "FancyAssert.h"
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager() {
   broadphase = UPtr<btDbvtBroadphase>(new btDbvtBroadphase);
   collisionConfiguration = UPtr<btCollisionConfiguration>(new btDefaultCollisionConfiguration);
   collisionDispatcher = UPtr<btCollisionDispatcher>(new btCollisionDispatcher(collisionConfiguration.get()));
   constraintSolver = UPtr<btConstraintSolver>(new btSequentialImpulseConstraintSolver);
   dynamicsWorld = UPtr<btDynamicsWorld>(new btDiscreteDynamicsWorld(collisionDispatcher.get(), broadphase.get(), constraintSolver.get(), collisionConfiguration.get()));
   dynamicsWorld->setGravity(DEFAULT_GRAVITY);
}

PhysicsManager::~PhysicsManager() {
   // Clean up in reverse order
   dynamicsWorld.reset();
   constraintSolver.reset();
   collisionDispatcher.reset();
   collisionConfiguration.reset();
   broadphase.reset();
}

void PhysicsManager::tick(const float dt) {
   dynamicsWorld->stepSimulation(dt, 15);
}

void PhysicsManager::addObject(PhysicsComponent &physicsComponent) {
   btRigidBody *rigidBody = physicsComponent.getRigidBody();
   ASSERT(rigidBody, "Trying to add null body to PhysicsManager");
   if (rigidBody) {
      dynamicsWorld->addRigidBody(rigidBody);
      physicsComponent.onAdd(shared_from_this());
   }
}

void PhysicsManager::removeObject(PhysicsComponent &physicsComponent) {
   btRigidBody *rigidBody = physicsComponent.getRigidBody();
   ASSERT(rigidBody, "Trying to remove null body from PhysicsManager");
   if (rigidBody) {
      dynamicsWorld->removeRigidBody(rigidBody);
   }
}

btDynamicsWorld& PhysicsManager::getDynamicsWorld() const {
   return *dynamicsWorld;
}
