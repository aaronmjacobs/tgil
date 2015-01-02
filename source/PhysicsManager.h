#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include <bullet/btBulletDynamicsCommon.h>

const btVector3 DEFAULT_GRAVITY(0.0f, -9.8f, 0.0f);

class PhysicsManager {
protected:
   btBroadphaseInterface *broadphase;
   btCollisionConfiguration *collisionConfiguration;
   btCollisionDispatcher *collisionDispatcher;
   btConstraintSolver *constraintSolver;
   btDynamicsWorld *dynamicsWorld;

public:
   PhysicsManager();

   virtual ~PhysicsManager();

   btDynamicsWorld& getDynamicsWorld();
};

#endif
