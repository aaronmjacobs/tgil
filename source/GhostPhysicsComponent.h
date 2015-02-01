#ifndef GHOST_PHYSICS_COMPONENT_H
#define GHOST_PHYSICS_COMPONENT_H

#include "PhysicsComponent.h"

#include <glm/glm.hpp>

class btCollisionShape;
class btMotionState;

class GhostPhysicsComponent : public PhysicsComponent {
private:
   void initCollisionObject(const bool dynamic);

public:
   GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short collisionMask);

   GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short collisionMask, const float radius);

   GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short collisionMask, const glm::vec3 &halfExtents);

   virtual ~GhostPhysicsComponent();
};

#endif
