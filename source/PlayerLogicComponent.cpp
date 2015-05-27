#include "CameraComponent.h"
#include "Conversions.h"
#include "Context.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "PlayerCameraComponent.h"
#include "PlayerGraphicsComponent.h"
#include "PlayerLogicComponent.h"
#include "Scene.h"
#include "ShoveAbility.h"
#include "ThrowAbility.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionMargin.h>

// For some reason, min and max are defined on Windows, which conflicts with glm::min / glm::max
#ifdef _WIN32
#undef min
#undef max
#endif // _WIN32

namespace {

// Look constants
const float LOOK_SPEED = 5.0f;
const float Y_LOOK_BOUND = 0.99f;
const float BOB_AMOUNT = 1.0f / 30.0f;

// Movement constants
const float MAX_MOVE_FORCE = 240.0f;
const float NORMAL_MOVE_FORCE = 240.0f;
const float AIR_MOVE_MULTIPLIER = 0.4f;
const float MAX_AIR_MOVE_SPEED = 9.0f;
const float JUMP_IMPULSE = 10.0f;
const float STEP_DISTANCE = 2.5f;
const float GROUND_JUMP_TIME = 0.25f;

// Ground / friction constants
const float FRICTION_CONSTANT = 30.0f;
const float DEFAULT_GROUND_FRICTION = 1.0f;

// Spring constants
const float SPRING_FORCE_MULTIPLIER = 100.0f;
const float SPRING_STIFFNESS = 4.0f;
const float SPRING_DAMPING = 0.3f;
const float SPRING_HEIGHT = 0.6f;
const float SPRING_RAYCAST_MARGIN = 0.5f;

// Appendage constants
const glm::vec3 HEAD_OFFSET(0.0f, 0.75f, 0.0f);
const glm::vec3 LEFT_HAND_OFFSET(-0.65f, 0.0f, -0.5f);
const glm::vec3 RIGHT_HAND_OFFSET(0.65f, 0.0f, -0.5f);
const glm::vec3 LEFT_FOOT_OFFSET(-0.25f, -1.5f, 0.0f);
const glm::vec3 RIGHT_FOOT_OFFSET(0.25f, -1.5f, 0.0f);
const glm::vec3 FOOT_RAYCAST_OFFSET(0.0f, 3.0f, 0.0f);
const float APPENDAGE_STIFFNESS = 16.0f;
const float HAND_STIFFNESS = 6.0f;
const float APPENDAGE_DAMPING = 0.2f;
const float FOOT_STEP_RADIUS = 0.5f;
const float FOOT_LOWER_TIME = 0.25f;
const float FOOT_STEP_DISTANCE = 1.5f;
const float HAND_OFFSET_MULTIPLIER = 0.25f;
const float ABILITY_ANIMATION_TIME = 0.2f;
const float ABILITY_ANIMATION_OFFSET = 2.0f;
const float ABILITY_ANIMATION_Y_MULTIPLIER = 0.25f;

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

glm::vec3 springForce(const glm::vec3 &springPos, const glm::vec3 &objectPos, const glm::vec3 &objectVel, float stiffness, float damping) {
   return -stiffness * (objectPos - springPos) - damping * objectVel;
}

glm::vec3 footPos(const glm::vec3 &playerPos, const glm::vec3 &baseFootPos, SPtr<Scene> scene) {
   glm::vec3 baseFootWorldPos = baseFootPos + playerPos;
   btVector3 from(toBt(baseFootWorldPos) + toBt(FOOT_RAYCAST_OFFSET));
   btVector3 to(toBt(baseFootWorldPos) - toBt(FOOT_RAYCAST_OFFSET));

   btCollisionWorld::ClosestRayResultCallback callback(from, to);
   callback.m_collisionFilterMask = CollisionGroup::Default | CollisionGroup::StaticBodies;
   scene->getPhysicsManager()->getDynamicsWorld().rayTest(from, to, callback);

   if (!callback.hasHit() || (callback.m_hitPointWorld.y() - playerPos.y) < baseFootPos.y || (callback.m_hitPointWorld.y() > playerPos.y)) {
      return baseFootPos;
   }

   return toGlm(callback.m_hitPointWorld) - playerPos;
}

} // namespace

PlayerLogicComponent::PlayerLogicComponent(GameObject &gameObject, int playerNum, const glm::vec3 &color)
   : LogicComponent(gameObject), playerNum(playerNum), alive(true), wasJumpingLastFrame(false), canDoubleJump(false), footSwap(false), jumpTimer(0.0f), distanceSinceStep(STEP_DISTANCE), deathTime(0.0f), timeSinceMovement(0.0f), timeSincePrimary(1.0f), timeSinceSecondary(1.0f), color(color), primaryAbility(std::make_shared<ThrowAbility>(gameObject)), secondaryAbility(std::make_shared<ShoveAbility>(gameObject)) {
}

PlayerLogicComponent::~PlayerLogicComponent() {
}

folly::Optional<Ground> PlayerLogicComponent::getGround(SPtr<PhysicsManager> physicsManager) const {
   PhysicsComponent &physicsComponent = gameObject.getPhysicsComponent();
   btCollisionObject *playerCollisionObject = physicsComponent.getCollisionObject();

   // Get the player's AABB
   btCollisionShape *playerShape = playerCollisionObject->getCollisionShape();
   ASSERT(playerShape, "Player must have collision shape");
   btVector3 playerMin, playerMax;
   playerShape->getAabb(playerCollisionObject->getWorldTransform(), playerMin, playerMax);

   // Fire a ray downwards to see if the player is standing on something
   btVector3 from(playerMin + btVector3(0.0f, 0.5f, 0.0f));
   btVector3 to(toBt(toGlm(playerMin) - glm::vec3(0.0f, SPRING_HEIGHT + SPRING_RAYCAST_MARGIN, 0.0f)));
   btCollisionWorld::ClosestRayResultCallback callback(from, to);
   callback.m_collisionFilterMask = CollisionGroup::Default | CollisionGroup::StaticBodies;
   physicsManager->getDynamicsWorld().rayTest(from, to, callback);

   if (!callback.hasHit()) {
      return folly::none;
   }

   return Ground(callback.m_hitPointWorld.y() + CONVEX_DISTANCE_MARGIN, // y position of ground
                 callback.m_collisionObject->getFriction(),             // friction of ground
                 toGlm(callback.m_hitNormalWorld));                     // normal vector of ground
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
   float desiredDist = SPRING_HEIGHT;
   float actualDist = playerMin.y() - ground.y;
   float yVel = rigidBody->getLinearVelocity().y();

   // Calculate the spring force
   float springForce = -SPRING_STIFFNESS * (glm::abs(actualDist) - desiredDist) - SPRING_DAMPING * yVel;
   float yForce = springForce * SPRING_FORCE_MULTIPLIER * (1.0f  / rigidBody->getInvMass());

   float currentYVel = rigidBody->getLinearVelocity().y();

   // Don't apply the force if the player is moving upwards and we are trying to pull them downwards
   if (yForce < 0.0f && currentYVel > 0.0f) {
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
      jumpTimer = GROUND_JUMP_TIME;
   }

   if (inputValues.jump && !wasJumpingLastFrame && (jumpTimer > 0.0f || canDoubleJump)) {
      wasJumpingLastFrame = true;

      // Cancel out any negative Y movement
      float cancelNegativeY = glm::max(0.0f, -velocity.y);
      jumpImpulse += glm::vec3(0.0f, cancelNegativeY, 0.0f);

      // Add in the jump impulse
      jumpImpulse += glm::vec3(0.0f, JUMP_IMPULSE, 0.0f);

      // Check if it is a double jump
      if (jumpTimer <= 0.0f && canDoubleJump) {
         canDoubleJump = false;

         glm::vec3 horizontalVelocity = velocity;
         horizontalVelocity.y = 0.0f;

         // Apply a horizontal impulse at a max of the square root of the current horizontal velocity,
         // based on the current horizontal velocity and where the player is trying to go
         float relation = calcRelation(horizontalVelocity, horizontalMoveIntention);
         float horizontalImpulseMultiplier = relation * glm::sqrt(glm::length(horizontalVelocity));
         jumpImpulse += horizontalMoveIntention * horizontalImpulseMultiplier;
      }

      jumpTimer = 0.0f;
      gameObject.notify(gameObject, Event::JUMP);
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

   // Check to see if the spring is colliding with some sort of ground
   folly::Optional<Ground> ground = getGround(scene->getPhysicsManager());
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

      if (distanceSinceStep >= STEP_DISTANCE || jumpTimer < 0.0f) {
         distanceSinceStep = 0.0f;
         footSwap = !footSwap;
         gameObject.notify(gameObject, Event::STEP);
      }

      PlayerCameraComponent *cameraComponent = dynamic_cast<PlayerCameraComponent*>(&gameObject.getCameraComponent());
      if (cameraComponent) {
         float offset = (distanceSinceStep / STEP_DISTANCE) * (2.0f * glm::pi<float>()) - glm::half_pi<float>();
         cameraComponent->setHeadOffset(sin(offset) * BOB_AMOUNT);
      }
   }

   // Calculate the horizontal movement force
   glm::vec3 horizontalMoveIntention = calcMoveIntention(inputValues);
   float horizontalForceAmount = calcHorizontalMovementForce(toGlm(velocity), horizontalMoveIntention, ground);
   glm::vec3 horizontalForce = horizontalMoveIntention * glm::vec3(horizontalForceAmount);
   netForce += toBt(horizontalForce);

   // Calculate the jump impulse
   netImpulse += toBt(calcJumpImpulse(inputValues, toGlm(velocity), horizontalMoveIntention, ground.hasValue()));
   jumpTimer -= dt;

   // Apply the impulses and forces
   rigidBody->applyCentralImpulse(netImpulse);
   rigidBody->applyCentralForce(netForce);

   handleAppendages(dt, ground, scene);
}

void PlayerLogicComponent::handleAttack(const float dt, const InputValues &inputValues, SPtr<Scene> scene) {
   primaryAbility->tick(dt);
   secondaryAbility->tick(dt);

   if (inputValues.primaryAttack) {
      if (primaryAbility->use()) {
         timeSincePrimary = 0.0f;
      }
   }

   if (inputValues.secondaryAttack) {
      if (secondaryAbility->use()) {
         timeSinceSecondary = 0.0f;
      }
   }
}

void PlayerLogicComponent::handleAppendages(const float dt, folly::Optional<Ground> ground, SPtr<Scene> scene) {
   PlayerGraphicsComponent *playerGraphics = dynamic_cast<PlayerGraphicsComponent*>(&gameObject.getGraphicsComponent());
   if (!playerGraphics) {
      return;
   }

   btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(gameObject.getPhysicsComponent().getCollisionObject());
   ASSERT(rigidBody, "Player rigid body should not be null");
   if (!rigidBody) {
      return;
   }

   glm::vec3 playerVel = toGlm(rigidBody->getLinearVelocity());
   if (glm::length(playerVel) > 0.25f) {
      timeSinceMovement = 0.0f;
   } else {
      timeSinceMovement += dt;
   }

   glm::quat rot = gameObject.getOrientation();
   rot.x = 0.0f;
   rot.z = 0.0f;
   glm::quat playerOrientation = glm::normalize(rot);

   // Calculate limb offsets
   float offset = (distanceSinceStep / STEP_DISTANCE) * (1.0f * glm::pi<float>()) - glm::half_pi<float>();
   offset *= footSwap ? -1.0f : 1.0f;
   float footStep = glm::sin(offset) * FOOT_STEP_RADIUS;
   float footHeight = glm::cos(offset) * FOOT_STEP_RADIUS * (footSwap ? 1.0f : -1.0f);
   float footLowerAmount = -glm::smoothstep(0.0f, FOOT_LOWER_TIME, timeSinceMovement) * FOOT_STEP_RADIUS;

   glm::vec3 horizontalVel(playerVel.x, 0.0f, playerVel.z);
   float horizontalSpeed = glm::length(horizontalVel);
   glm::vec3 footOffset(0.0f);
   if (horizontalSpeed > 0.0f) {
      float footOffsetAmount = glm::smoothstep(0.0f, 1.0f, horizontalSpeed) * FOOT_STEP_DISTANCE;
      footOffset = glm::normalize(horizontalVel) * footOffsetAmount * glm::vec3(footStep);
      footOffset = -footOffset * glm::inverse(playerOrientation);
   }
   glm::vec3 limbOffset = glm::vec3(footOffset.x, footHeight, footOffset.z);
   glm::vec3 handOffset = glm::vec3(0.0f, -footStep, footStep) * HAND_OFFSET_MULTIPLIER;

   float primaryAttackAmount = timeSincePrimary < ABILITY_ANIMATION_TIME ? ABILITY_ANIMATION_OFFSET : 0.0f;
   float secondaryAttackAmount = timeSinceSecondary < ABILITY_ANIMATION_TIME ? ABILITY_ANIMATION_OFFSET : 0.0f;
   timeSincePrimary += dt;
   timeSinceSecondary += dt;

   glm::vec3 rightHandOffset(handOffset + glm::vec3(0.0f, (primaryAttackAmount + secondaryAttackAmount) * ABILITY_ANIMATION_Y_MULTIPLIER, -(primaryAttackAmount + secondaryAttackAmount)));
   glm::vec3 leftHandOffset(-handOffset + glm::vec3(0.0f, secondaryAttackAmount * ABILITY_ANIMATION_Y_MULTIPLIER, -secondaryAttackAmount));

   glm::vec3 headSpringPos(HEAD_OFFSET * playerOrientation);
   glm::vec3 leftHandSpringPos((LEFT_HAND_OFFSET + leftHandOffset) * playerOrientation);
   glm::vec3 rightHandSpringPos((RIGHT_HAND_OFFSET + rightHandOffset) * playerOrientation);
   glm::vec3 leftFootOffset(limbOffset + glm::vec3(0.0f, footLowerAmount, 0.0f));
   glm::vec3 leftFootSpringPos((LEFT_FOOT_OFFSET + leftFootOffset) * playerOrientation);
   glm::vec3 rightFootOffset(-limbOffset + glm::vec3(0.0f, footLowerAmount, 0.0f));
   glm::vec3 rightFootSpringPos((RIGHT_FOOT_OFFSET + rightFootOffset) * playerOrientation);

   // Raycast to find feet positions
   leftFootSpringPos = footPos(gameObject.getPosition(), leftFootSpringPos, scene);
   rightFootSpringPos = footPos(gameObject.getPosition(), rightFootSpringPos, scene);

   glm::vec3 headPos(playerGraphics->getHeadOffset());
   glm::vec3 leftHandPos(playerGraphics->getLeftHandOffset());
   glm::vec3 rightHandPos(playerGraphics->getRightHandOffset());
   glm::vec3 leftFootPos(playerGraphics->getLeftFootOffset());
   glm::vec3 rightFootPos(playerGraphics->getRightFootOffset());

   // Apply spring forces, calculate new positions
   glm::vec3 headForce(springForce(headSpringPos, headPos, playerVel, APPENDAGE_STIFFNESS, APPENDAGE_DAMPING));
   glm::vec3 leftHandForce(springForce(leftHandSpringPos, leftHandPos, playerVel, HAND_STIFFNESS, APPENDAGE_DAMPING));
   glm::vec3 rightHandForce(springForce(rightHandSpringPos, rightHandPos, playerVel, HAND_STIFFNESS, APPENDAGE_DAMPING));
   glm::vec3 leftFootForce(springForce(leftFootSpringPos, leftFootPos, playerVel, APPENDAGE_STIFFNESS, APPENDAGE_DAMPING));
   glm::vec3 rightFootForce(springForce(rightFootSpringPos, rightFootPos, playerVel, APPENDAGE_STIFFNESS, APPENDAGE_DAMPING));

   // Set new positions
   playerGraphics->setHeadOffset(headPos + (headForce * dt));
   playerGraphics->setLeftHandOffset(leftHandPos + (leftHandForce * dt));
   playerGraphics->setRightHandOffset(rightHandPos + (rightHandForce * dt));
   playerGraphics->setLeftFootOffset(leftFootPos + (leftFootForce * dt));
   playerGraphics->setRightFootOffset(rightFootPos + (rightFootForce * dt));
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

void PlayerLogicComponent::setAlive(bool alive) {
   if (this->alive == alive) {
      return;
   }

   this->alive = alive;

   if (!alive) {
      gameObject.notify(gameObject, Event::DIE);
      deathTime = Context::getInstance().getRunningTime();
   }
}

float PlayerLogicComponent::timeSinceDeath() const {
   return Context::getInstance().getRunningTime() - deathTime;
}

const Ability& PlayerLogicComponent::getPrimaryAbility() const {
   return *primaryAbility;
}

const Ability& PlayerLogicComponent::getSecondaryAbility() const {
   return *secondaryAbility;
}
