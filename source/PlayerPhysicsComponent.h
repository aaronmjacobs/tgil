#ifndef PLAYER_PHYSICS_COMPONENT_H
#define PLAYER_PHYSICS_COMPONENT_H

#include "Observer.h"
#include "PhysicsComponent.h"

class btCollisionShape;
class btGhostObject;
class btMotionState;

namespace {

const float PLAYER_RADIUS = 0.25f;
const float PLAYER_MIDDLE_HEIGHT = 0.75f;
const float PLAYER_TOTAL_HEIGHT = PLAYER_MIDDLE_HEIGHT + 2 * PLAYER_RADIUS;

const float PLAYER_GHOST_RADIUS = PLAYER_RADIUS * 0.1f;
const float PLAYER_GHOST_MIDDLE_HEIGHT = 0.5f;
const float PLAYER_GHOST_TOTAL_HEIGHT = PLAYER_GHOST_MIDDLE_HEIGHT + 2.0f * PLAYER_GHOST_RADIUS;
const float PLAYER_GHOST_OFFSET = (PLAYER_TOTAL_HEIGHT / 2.0f) + (PLAYER_GHOST_TOTAL_HEIGHT / 2.0f);

} // namespace

class PlayerPhysicsComponent : public PhysicsComponent {
protected:
   UPtr<btMotionState> motionState;
   SPtr<btGhostObject> ghostObject;
   UPtr<btCollisionShape> ghostCollisionShape;

public:
   PlayerPhysicsComponent(GameObject &gameObject, float mass);

   virtual ~PlayerPhysicsComponent();

   virtual void addToManager(SPtr<PhysicsManager> manager);

   virtual void removeFromManager(SPtr<PhysicsManager> manager);

   virtual void onNotify(const GameObject &gameObject, Event event);

   btGhostObject& getGhostObject() const;
};

#endif
