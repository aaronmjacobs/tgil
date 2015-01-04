#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "Types.h"

#include <bullet/btBulletDynamicsCommon.h>

class PhysicsComponent;

const btVector3 DEFAULT_GRAVITY(0.0f, -9.8f, 0.0f);

class PhysicsManager : public std::enable_shared_from_this<PhysicsManager> {
protected:
   UPtr<btBroadphaseInterface> broadphase;
   UPtr<btCollisionConfiguration> collisionConfiguration;
   UPtr<btCollisionDispatcher> collisionDispatcher;
   UPtr<btConstraintSolver> constraintSolver;
   UPtr<btDynamicsWorld> dynamicsWorld;

public:
   PhysicsManager();

   virtual ~PhysicsManager();

   virtual void tick(const float dt);

   virtual void addObject(PhysicsComponent &physicsComponent);

   virtual void removeObject(PhysicsComponent &physicsComponent);

   btDynamicsWorld& getDynamicsWorld() const;
};

#endif
