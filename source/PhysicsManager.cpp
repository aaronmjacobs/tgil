#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"

PhysicsManager::PhysicsManager() {
   broadphase = new btDbvtBroadphase;
   collisionConfiguration = new btDefaultCollisionConfiguration;
   collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
   constraintSolver = new btSequentialImpulseConstraintSolver;
   dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionConfiguration);
   dynamicsWorld->setGravity(DEFAULT_GRAVITY);
}

PhysicsManager::~PhysicsManager() {
   delete dynamicsWorld;
   delete constraintSolver;
   delete collisionDispatcher;
   delete collisionConfiguration;
   delete broadphase;
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

btDynamicsWorld& PhysicsManager::getDynamicsWorld() {
   return *dynamicsWorld;
}
