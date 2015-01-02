#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "Types.h"

#include <bullet/btBulletDynamicsCommon.h>

class GameObject;

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

   virtual void tick(const float dt);

   virtual void addObject(SPtr<GameObject> gameObject);

   virtual void removeObject(SPtr<GameObject> gameObject);

   btDynamicsWorld& getDynamicsWorld();
};

#endif
