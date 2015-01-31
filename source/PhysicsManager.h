#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "Types.h"

class btBroadphaseInterface;
class btCollisionConfiguration;
class btCollisionDispatcher;
class btConstraintSolver;
class btDynamicsWorld;
class btGhostPairCallback;
class btIDebugDraw;
class PhysicsComponent;

class PhysicsManager : public std::enable_shared_from_this<PhysicsManager> {
protected:
   UPtr<btBroadphaseInterface> broadphase;
   UPtr<btCollisionConfiguration> collisionConfiguration;
   UPtr<btCollisionDispatcher> collisionDispatcher;
   UPtr<btConstraintSolver> constraintSolver;
   UPtr<btDynamicsWorld> dynamicsWorld;
   UPtr<btGhostPairCallback> ghostPairCallback;

public:
   PhysicsManager();

   virtual ~PhysicsManager();

   void setDebugDrawer(btIDebugDraw *debugDrawer);

   void debugDraw();

   virtual void tick(const float dt);

   btDynamicsWorld& getDynamicsWorld() const;
};

#endif
