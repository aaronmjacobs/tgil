#ifndef TRIGGER_VOLUME_PHYSICS_COMPONENT_H
#define TRIGGER_VOLUME_PHYSICS_COMPONENT_H

#include "PhysicsComponent.h"

#include <glm/glm.hpp>

class btCollisionShape;
class btMotionState;

class TriggerVolumePhysicsComponent : public PhysicsComponent {
protected:
   UPtr<btCollisionShape> collisionShape;
   UPtr<btMotionState> motionState;

public:
   TriggerVolumePhysicsComponent(GameObject &gameObject, glm::vec3 halfExtents, const CollisionGroup::Group mask);

   virtual ~TriggerVolumePhysicsComponent();
};

#endif
