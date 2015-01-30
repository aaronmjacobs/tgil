#ifndef GHOST_PHYSICS_COMPONENT_H
#define GHOST_PHYSICS_COMPONENT_H

#include "PhysicsComponent.h"

#include <glm/glm.hpp>

class btCollisionShape;
class btMotionState;

class GhostPhysicsComponent : public PhysicsComponent {
private:
   void init();

public:
   GhostPhysicsComponent(GameObject &gameObject, const int mask, const bool dynamic);

   GhostPhysicsComponent(GameObject &gameObject, const int mask, const bool dynamic, const float radius);

   GhostPhysicsComponent(GameObject &gameObject, const int mask, const bool dynamic, const glm::vec3 &halfExtents);

   virtual ~GhostPhysicsComponent();
};

#endif
