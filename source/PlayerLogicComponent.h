#ifndef PLAYER_LOGIC_COMPONENT_H
#define PLAYER_LOGIC_COMPONENT_H

#include "LogicComponent.h"

#include <folly/Optional.h>
#include <glm/glm.hpp>

class Ability;
class btRigidBody;
class PhysicsManager;

struct Ground {
   const float y;
   const float friction;
   const glm::vec3 normal;

   Ground(float y, float friction, glm::vec3 normal)
      : y(y), friction(friction), normal(normal) {
   }
};

class PlayerLogicComponent : public LogicComponent {
protected:
   int playerNum;
   bool alive;
   bool wasJumpingLastFrame;
   bool canDoubleJump;
   bool footSwap;
   float jumpTimer;
   float distanceSinceStep;
   float deathTime;
   float timeSinceMovement;
   float timeSincePrimary;
   float timeSinceSecondary;
   glm::vec3 color;
   SPtr<Ability> primaryAbility;
   SPtr<Ability> secondaryAbility;

   folly::Optional<Ground> getGround(SPtr<PhysicsManager> physicsManager) const;

   folly::Optional<glm::vec3> calcSpringForce(const Ground &ground, const btRigidBody *rigidBody) const;

   glm::vec3 calcMoveIntention(const InputValues &inputValues) const;

   glm::vec3 calcJumpImpulse(const InputValues &inputValues, const glm::vec3 &velocity, const glm::vec3 &horizontalMoveIntention, bool onGround);

   void handleOrientation(const float dt, const InputValues &inputValues);

   void handleMovement(const float dt, const InputValues &inputValues, SPtr<Scene> scene);

   void handleAttack(const float dt, const InputValues &inputValues, SPtr<Scene> scene);

   void handleAppendages(const float dt, folly::Optional<Ground> ground, SPtr<Scene> scene);

public:
   PlayerLogicComponent(GameObject &gameObject, int playerNum, const glm::vec3 &color);

   virtual ~PlayerLogicComponent();

   virtual void tick(const float dt);

   int getPlayerNum() const {
      return playerNum;
   }

   const glm::vec3& getColor() const {
      return color;
   }

   bool isAlive() const {
      return alive;
   }

   void setAlive(bool alive);

   float timeSinceDeath() const;

   const Ability &getPrimaryAbility() const;

   const Ability &getSecondaryAbility() const;
};

#endif
