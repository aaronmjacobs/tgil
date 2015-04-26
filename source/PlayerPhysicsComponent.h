#ifndef PLAYER_PHYSICS_COMPONENT_H
#define PLAYER_PHYSICS_COMPONENT_H

#include "Observer.h"
#include "PhysicsComponent.h"

class btCollisionShape;
class btMotionState;

namespace {

const float PLAYER_RADIUS = 0.5f;
const float PLAYER_MIDDLE_HEIGHT = 0.75f;
const float PLAYER_TOTAL_HEIGHT = PLAYER_MIDDLE_HEIGHT + 2.0f * PLAYER_RADIUS;

} // namespace

class PlayerPhysicsComponent : public PhysicsComponent {
protected:
   UPtr<btMotionState> motionState;

public:
   PlayerPhysicsComponent(GameObject &gameObject, float mass);

   virtual ~PlayerPhysicsComponent();

   glm::vec3 getVelocity() const;
};

#endif
