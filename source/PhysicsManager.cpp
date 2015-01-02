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

btDynamicsWorld& PhysicsManager::getDynamicsWorld() {
   return *dynamicsWorld;
}
