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

void PhysicsManager::addObject(SPtr<GameObject> gameObject) {
   btRigidBody *rigidBody = gameObject->getPhysicsComponent().getRigidBody();
   if (rigidBody) {
      dynamicsWorld->addRigidBody(rigidBody);
   }
}

void PhysicsManager::removeObject(SPtr<GameObject> gameObject) {
   btRigidBody *rigidBody = gameObject->getPhysicsComponent().getRigidBody();
   if (rigidBody) {
      dynamicsWorld->removeRigidBody(rigidBody);
   }
}

btDynamicsWorld& PhysicsManager::getDynamicsWorld() const {
   return *dynamicsWorld;
}
