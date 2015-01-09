#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "PlayerCameraComponent.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/gtc/matrix_transform.hpp>

namespace {

const float LOOK_SPEED = 5.0f;
const float Y_LOOK_BOUND = 0.99f;

// TODO Calculate based off of physics property of player
const float MAX_MOVE_FORCE = 400.0f;
const float NORMAL_MOVE_FORCE = 150.0f;
const float JUMP_FORCE = 150.0f;

// TODO Calculate based off of physics property of player
const float GROUND_DISTANCE = 0.8f;
const float HEAD_OFFSET = 0.7f;

bool onGround(SPtr<Scene> scene, const btRigidBody &rigidBody) {
   SPtr<PhysicsManager> physicsManager = scene->getPhysicsManager();
   btDynamicsWorld &world = physicsManager->getDynamicsWorld();

   btTransform trans;
   rigidBody.getMotionState()->getWorldTransform(trans);

   btVector3 from = trans.getOrigin();
   btVector3 to(from.x(), -10000.0, from.z());
   btCollisionWorld::ClosestRayResultCallback res(from, to);

   world.rayTest(from, to, res);
   if(!res.hasHit()) {
      return false;
   }

   btVector3 hitPoint = res.m_hitPointWorld;
   return glm::distance(toGlm(from), toGlm(to)) <= GROUND_DISTANCE;
}

} // namespace

PlayerCameraComponent::~PlayerCameraComponent() {
}

void PlayerCameraComponent::tick(GameObject &gameObject, const float dt) {
   SPtr<Scene> scene = gameObject.getScene().lock();
   ASSERT(scene, "PlayerCameraComponent must be in Scene to tick");
   if (!scene) {
      return;
   }

   btRigidBody *rigidBody = gameObject.getPhysicsComponent().getRigidBody();
   ASSERT(rigidBody, "Player rigid body should not be null");
   if (!rigidBody) {
      return;
   }

   const InputValues &inputValues = gameObject.getInputComponent().getInputValues(gameObject);

   // Camera orientation
   float lookAmount = dt * LOOK_SPEED;
   float pitchAmount = lookAmount * inputValues.lookY;
   float yawAmount = lookAmount * inputValues.lookX;
   glm::vec3 front = getFrontVector(gameObject);
   glm::vec3 right = getRightVector(gameObject);

   if (front.y - pitchAmount > Y_LOOK_BOUND) {
      pitchAmount = front.y - Y_LOOK_BOUND;
   }
   if (front.y - pitchAmount < -Y_LOOK_BOUND) {
      pitchAmount = front.y + Y_LOOK_BOUND;
   }

   glm::quat pitchChange = glm::angleAxis(pitchAmount, glm::vec3(1.0f, 0.0f, 0.0f));
   glm::quat yawChange = glm::angleAxis(yawAmount, glm::vec3(0.0f, 1.0f, 0.0f));
   gameObject.setOrientation(glm::normalize(pitchChange * gameObject.getOrientation() * yawChange));

   // Player movement
   glm::vec3 flatFront = front;
   flatFront.y = 0.0f;
   flatFront = glm::normalize(flatFront);

   glm::vec3 flatRight = right;
   flatRight.y = 0.0f;
   flatRight = glm::normalize(flatRight);

   glm::vec3 moveIntention = (((inputValues.moveForward - inputValues.moveBackward) * flatFront) + ((inputValues.moveRight - inputValues.moveLeft) * flatRight));
   if (glm::length(moveIntention) > 1.0f) {
      // Don't let the player move faster by going diagonally
      moveIntention = glm::normalize(moveIntention);
   }

   if (onGround(scene, *rigidBody)) {
      btVector3 velocity = rigidBody->getLinearVelocity();
      float speed = glm::length(glm::vec3(velocity.x(), 0.0f, velocity.z()));

      // TODO Deal with speed / force per direction, not just as one constant
      float forceAmount = speed < (NORMAL_MOVE_FORCE / MAX_MOVE_FORCE) ? MAX_MOVE_FORCE : glm::min(MAX_MOVE_FORCE, NORMAL_MOVE_FORCE / speed);
      glm::vec3 force = moveIntention * forceAmount;
      rigidBody->applyCentralForce(btVector3(force.x, force.y, force.z));

      if (inputValues.jump) {
         rigidBody->applyCentralForce(btVector3(0.0f, JUMP_FORCE, 0.0f));
      }
   }
}

glm::vec3 PlayerCameraComponent::getFrontVector(GameObject &gameObject) const {
   return glm::vec3(0.0f, 0.0f, -1.0f) * glm::normalize(gameObject.getOrientation());
}

glm::vec3 PlayerCameraComponent::getRightVector(GameObject &gameObject) const {
   return glm::vec3(1.0f, 0.0f, 0.0f) * glm::normalize(gameObject.getOrientation());
}

glm::mat4 PlayerCameraComponent::getViewMatrix(GameObject &gameObject) const {
   const glm::vec3 position = getCameraPosition(gameObject);
   return glm::lookAt(position,
                      position + getFrontVector(gameObject),
                      glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector
}

glm::vec3 PlayerCameraComponent::getCameraPosition(GameObject &gameObject) const {
   return gameObject.getPosition() + glm::vec3(0.0f, HEAD_OFFSET, 0.0f);
}
