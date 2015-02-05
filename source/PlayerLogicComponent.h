#ifndef PLAYER_LOGIC_COMPONENT_H
#define PLAYER_LOGIC_COMPONENT_H

#include "LogicComponent.h"

#include <folly/Optional.h>
#include <glm/glm.hpp>

class btRigidBody;

struct Ground {
   float maxY;
   float friction;

   Ground(float maxY, float friction)
      : maxY(maxY), friction(friction) {
   }
};

class PlayerLogicComponent : public LogicComponent {
protected:
   bool wasJumpingLastFrame;
   glm::vec3 color;

   folly::Optional<Ground> getGround() const;

   folly::Optional<glm::vec3> calcSpringForce(const Ground &ground, const btRigidBody *rigidBody) const;

   glm::vec3 calcMoveIntention(const InputValues &inputValues) const;

   void handleOrientation(const float dt, const InputValues &inputValues);

   void handleMovement(const float dt, const InputValues &inputValues, SPtr<Scene> scene);

public:
   PlayerLogicComponent(GameObject &gameObject, const glm::vec3 &color);

   virtual ~PlayerLogicComponent();

   virtual void tick(const float dt);

   const glm::vec3& getColor() {
      return color;
   }
};

#endif
