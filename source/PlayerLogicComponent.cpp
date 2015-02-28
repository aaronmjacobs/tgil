#include "CameraComponent.h"
#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "InputComponent.h"
#include "PlayerLogicComponent.h"
#include "PlayerPhysicsComponent.h"
#include "Scene.h"
#include "ShoveAbility.h"
#include "ThrowAbility.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

// For some reason, min is defined on Windows, which conflicts with glm::min
#ifdef _WIN32
#undef min
#endif // _WIN32

namespace {

// Look constants
const float LOOK_SPEED = 5.0f;
const float Y_LOOK_BOUND = 0.99f;

// Movement constants
const float MAX_MOVE_FORCE = 150.0f;
const float NORMAL_MOVE_FORCE = 150.0f;
const float AIR_MOVE_MULTIPLIER = 0.2f;
const float MAX_AIR_MOVE_SPEED = 7.0f;
const float JUMP_IMPULSE = 7.0f;
const float STEP_DISTANCE = 1.3f;

// Ground / friction constants
const float FRICTION_CONSTANT = 30.0f;
const float DEFAULT_GROUND_FRICTION = 1.0f;
const float GROUND_HEIGHT_ADJUST = 0.1f;

// Spring constants
const float SPRING_FORCE_MULTIPLIER = 100.0f;
const float SPRING_STIFFNESS = 2.0f;
const float SPRING_DAMPING = 0.3f;

/**
 * Calculates how related the two given vectors are (in terms of direction), from 0 (same direction) to 1 (opposite directions)
*/
float calcRelation(const glm::vec3 &first, const glm::vec3 &second) {
   float relation = 0.0f;
   float firstLength = glm::length(second);
   float secondLength = glm::length(first);

   if (firstLength > 0.0f && secondLength > 0.0f) {
      relation = glm::dot(first, second) / (firstLength * secondLength);
      relation = (1.0f - relation) / 2.0f; // 0.0 = same direction, 1.0 = opposite
   }

   return relation;
}

float calcHorizontalMovementForce(glm::vec3 velocity, const glm::vec3 &horizontalMoveIntention, folly::Optional<Ground> ground) {
   velocity.y = 0.0f;
   float speed = glm::length(velocity);

   float forceModifier;
   if (ground) {
      forceModifier = ground->friction > 0.0f ? ground->friction : DEFAULT_GROUND_FRICTION;
   } else {
      forceModifier = AIR_MOVE_MULTIPLIER;

      // Don't allow players to move themselves too quickly through the air
      if (speed > MAX_AIR_MOVE_SPEED) {
         forceModifier *= calcRelation(velocity, horizontalMoveIntention);
      }
   }

   float normalMoveForce = NORMAL_MOVE_FORCE * forceModifier;
   float maxMoveForce = MAX_MOVE_FORCE * forceModifier;
   return speed < normalMoveForce / maxMoveForce ? maxMoveForce : glm::min(maxMoveForce, normalMoveForce / speed);
}

} // namespace

PlayerLogicComponent::PlayerLogicComponent(GameObject &gameObject, const glm::vec3 &color)
   : LogicComponent(gameObject), alive(true), wasJumpingLastFrame(false), canDoubleJump(false), distanceSinceStep(STEP_DISTANCE), color(color), primaryAbility(std::make_shared<ThrowAbility>(gameObject)), secondaryAbility(std::make_shared<ShoveAbility>(gameObject)) {
}

PlayerLogicComponent::~PlayerLogicComponent() {
}

folly::Optional<Ground> PlayerLogicComponent::getGround() const {
   PlayerPhysicsComponent *playerPhysicsComponent = static_cast<PlayerPhysicsComponent*>(&gameObject.getPhysicsComponent());
   ASSERT(playerPhysicsComponent, "Player should have PlayerPhysicsComponent");
   if (!playerPhysicsComponent) {
      return folly::none;
   }

   btCollisionObject *playerCollisionObject = playerPhysicsComponent->getCollisionObject();
   btGhostObject &ghost = playerPhysicsComponent->getGhostObject();

   // Get the player's AABB
   btCollisionShape *playerShape = playerCollisionObject->getCollisionShape();
   ASSERT(playerShape, "Player must have collision shape");
   btVector3 playerMin, playerMax;
   playerShape->getAabb(playerCollisionObject->getWorldTransform(), playerMin, playerMax);

   // Try to find the best ground
   bool found = false;
   Ground ground(0.0f, 0.0f);
   for (int i = 0; i < ghost.getNumOverlappingObjects(); ++i) {
      btCollisionObject *object = ghost.getOverlappingObject(i);
      // Ignore the player
      if (object == playerCollisionObject) {
         continue;
      }

      btCollisionShape *shape = object->getCollisionShape();
      if (!shape) {
         continue;
      }

      // Get the object's AABB
      btVector3 min, max;
      shape->getAabb(object->getWorldTransform(), min, max);

      // Check if the object is below us, and if it is the highest 'ground'
      if (max.y() < playerMin.y() + GROUND_HEIGHT_ADJUST && (!found || max.y() > ground.maxY)) {
         found = true;
         ground.maxY = max.y();
         ground.friction = object->getFriction();
      }
   }

   if (!found) {
      return folly::none;
   }

   return ground;
}

folly::Optional<glm::vec3> PlayerLogicComponent::calcSpringForce(const Ground &ground, const btRigidBody *rigidBody) const {
   // Get the player's AABB
   const btCollisionShape *playerShape = rigidBody->getCollisionShape();
   ASSERT(playerShape, "Player must have collision shape");
   if (!playerShape) {
      return folly::none;
   }
   btVector3 playerMin, playerMax;
   playerShape->getAabb(rigidBody->getWorldTransform(), playerMin, playerMax);

   // Distance to ground and player velocity
   float desiredDist = PLAYER_GHOST_TOTAL_HEIGHT;
   float actualDist = playerMin.y() - ground.maxY;
   float yVel = rigidBody->getLinearVelocity().y();

   // Calculate the spring force
   float springForce = -SPRING_STIFFNESS * (glm::abs(actualDist) - desiredDist) - SPRING_DAMPING * yVel;
   float yForce = springForce * SPRING_FORCE_MULTIPLIER * (1.0f  / rigidBody->getInvMass());

   // Only apply the spring force if it is pushing upwards
   if (yForce <= 0.0f) {
      return folly::none;
   }

   return glm::vec3(0.0f, yForce, 0.0f);
}

glm::vec3 PlayerLogicComponent::calcMoveIntention(const InputValues &inputValues) const {
   CameraComponent &cameraComponent = gameObject.getCameraComponent();
   glm::vec3 front = cameraComponent.getFrontVector();
   glm::vec3 right = cameraComponent.getRightVector();

   // Remove the y component (movement in x-z plane)
   front.y = 0.0f;
   front = glm::normalize(front);
   right.y = 0.0f;
   right = glm::normalize(right);

   glm::vec3 moveIntention = (((inputValues.moveForward - inputValues.moveBackward) * front) + ((inputValues.moveRight - inputValues.moveLeft) * right));
   if (glm::length(moveIntention) > 1.0f) {
      // Don't let the player move faster by going diagonally
      moveIntention = glm::normalize(moveIntention);
   }

   return moveIntention;
}

glm::vec3 PlayerLogicComponent::calcJumpImpulse(const InputValues &inputValues, const glm::vec3 &velocity, const glm::vec3 &horizontalMoveIntention, bool onGround) {
   glm::vec3 jumpImpulse(0.0f);

   if (onGround) {
      canDoubleJump = true;
   }

   if (inputValues.jump && !wasJumpingLastFrame && (onGround || canDoubleJump)) {
      wasJumpingLastFrame = true;
      jumpImpulse += glm::vec3(0.0f, JUMP_IMPULSE, 0.0f);
      gameObject.notify(gameObject, Event::JUMP);

      if (!onGround && canDoubleJump) {
         canDoubleJump = false;

         // Cancel out any negative Y movement
         float cancelNegativeY = glm::max(0.0f, -velocity.y);
         jumpImpulse += glm::vec3(0.0f, cancelNegativeY, 0.0f);

         glm::vec3 horizontalVelocity = velocity;
         horizontalVelocity.y = 0.0f;

         // Apply a horizontal impulse at a max of the square root of the current horizontal velocity,
         // based on the current horizontal velocity and where the player is trying to go
         float relation = calcRelation(horizontalVelocity, horizontalMoveIntention);
         float horizontalImpulseMultiplier = relation * glm::sqrt(glm::length(horizontalVelocity));
         jumpImpulse += horizontalMoveIntention * horizontalImpulseMultiplier;
      }
   }

   if (!inputValues.jump) {
      wasJumpingLastFrame = false;
   }

   return jumpImpulse;
}

void PlayerLogicComponent::handleOrientation(const float dt, const InputValues &inputValues) {
   CameraComponent &cameraComponent = gameObject.getCameraComponent();
   float lookAmount = dt * LOOK_SPEED;
   float pitchAmount = lookAmount * inputValues.lookY;
   float yawAmount = lookAmount * inputValues.lookX;
   glm::vec3 front = cameraComponent.getFrontVector();

   // Prevent player from rotating past vertical bounds
   if (front.y - pitchAmount > Y_LOOK_BOUND) {
      pitchAmount = front.y - Y_LOOK_BOUND;
   }
   if (front.y - pitchAmount < -Y_LOOK_BOUND) {
      pitchAmount = front.y + Y_LOOK_BOUND;
   }

   glm::quat pitchChange = glm::angleAxis(pitchAmount, glm::vec3(1.0f, 0.0f, 0.0f));
   glm::quat yawChange = glm::angleAxis(yawAmount, glm::vec3(0.0f, 1.0f, 0.0f));
   gameObject.setOrientation(glm::normalize(pitchChange * gameObject.getOrientation() * yawChange));
}

void PlayerLogicComponent::handleMovement(const float dt, const InputValues &inputValues, SPtr<Scene> scene) {
   btCollisionObject* collisionObject = gameObject.getPhysicsComponent().getCollisionObject();
   btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(collisionObject);
   ASSERT(rigidBody, "Player rigid body should not be null");
   if (!rigidBody) {
      return;
   }

   btVector3 netImpulse(0.0f, 0.0f, 0.0f);
   btVector3 netForce(0.0f, 0.0f, 0.0f);
   btVector3 velocity = rigidBody->getLinearVelocity();

   // Check to see if the ghost is colliding with some sort of ground
   folly::Optional<Ground> ground = getGround();
   if (ground) {
      folly::Optional<glm::vec3> springForce = calcSpringForce(*ground, rigidBody);

      // Check if we're actually "standing" on the ground
      if (springForce) {
         // If so, apply friction / force from the spring

         btVector3 opposingVelocity = -velocity;
         if (opposingVelocity.length() > 1.0f) {
            opposingVelocity.normalize();
         }
         netForce += btVector3(opposingVelocity * FRICTION_CONSTANT);
         netForce += toBt(*springForce);
      } else {
         // If not, clear the ground
         ground = folly::none;
      }
   }

   // Step
   if (ground) {
      btVector3 horizontalVelocity = velocity;
      horizontalVelocity.setY(0.0f);

      distanceSinceStep += horizontalVelocity.length() * dt;

      if (distanceSinceStep >= STEP_DISTANCE) {
         distanceSinceStep = 0.0f;
         gameObject.notify(gameObject, Event::STEP);
      }
   }

   // Calculate the horizontal movement force
   glm::vec3 horizontalMoveIntention = calcMoveIntention(inputValues);
   float horizontalForceAmount = calcHorizontalMovementForce(toGlm(velocity), horizontalMoveIntention, ground);
   glm::vec3 horizontalForce = horizontalMoveIntention * glm::vec3(horizontalForceAmount);
   netForce += toBt(horizontalForce);

   // Calculate the jump impulse
   netImpulse += toBt(calcJumpImpulse(inputValues, toGlm(velocity), horizontalMoveIntention, ground.hasValue()));

   // Apply the impulses and forces
   rigidBody->applyCentralImpulse(netImpulse);
   rigidBody->applyCentralForce(netForce);
}

void PlayerLogicComponent::handleAttack(const float dt, const InputValues &inputValues, SPtr<Scene> scene) {
   primaryAbility->tick(dt);
   secondaryAbility->tick(dt);

   if (inputValues.primaryAttack) {
      primaryAbility->use();
   }

   if (inputValues.secondaryAttack) {
      secondaryAbility->use();
   }
}

void PlayerLogicComponent::tick(const float dt) {
   SPtr<Scene> scene = gameObject.getScene().lock();
   ASSERT(scene, "PlayerLogicComponent must be in Scene to tick");
   if (!scene) {
      return;
   }

   if (isAlive()) {
      const InputValues &inputValues = gameObject.getInputComponent().getInputValues();

      handleOrientation(dt, inputValues);
      handleMovement(dt, inputValues, scene);
      handleAttack(dt, inputValues, scene);
   }
}
