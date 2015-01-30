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
   GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short mask);

   GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short mask, const float radius);

   GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short mask, const glm::vec3 &halfExtents);

   virtual ~GhostPhysicsComponent();
};

#endif
