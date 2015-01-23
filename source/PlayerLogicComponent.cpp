#include "CameraComponent.h"
#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "InputComponent.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "PlayerLogicComponent.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>

// For some reason, min is defined on Windows, which conflicts with glm::min
#ifdef _WIN32
#undef min
#endif // _WIN32

namespace {

const float LOOK_SPEED = 5.0f;
const float Y_LOOK_BOUND = 0.99f;

// TODO Calculate based off of physics property of player
const float MAX_MOVE_FORCE = 150.0f;
const float NORMAL_MOVE_FORCE = 150.0f;
const float AIR_MOVE_MULTIPLIER = 0.05f;
const float JUMP_FORCE = 400.0f;

// TODO Calculate based off of physics property of player
const float GROUND_DISTANCE = 0.8f;

const btCollisionObject* getGround(SPtr<Scene> scene, const btRigidBody &rigidBody) {
   SPtr<PhysicsManager> physicsManager = scene->getPhysicsManager();
   btDynamicsWorld &world = physicsManager->getDynamicsWorld();

   btTransform trans = rigidBody.getWorldTransform();

   btVector3 from = trans.getOrigin();
   btVector3 to(from.x(), -10000.0, from.z());
   btCollisionWorld::ClosestRayResultCallback res(from, to);

   world.rayTest(from, to, res);
   if(!res.hasHit()) {
      return nullptr;
   }

   btVector3 hitPoint = res.m_hitPointWorld;
   if (glm::distance(toGlm(from), toGlm(hitPoint)) > GROUND_DISTANCE) {
      return nullptr;
   }

   return res.m_collisionObject;
}

float calcMovementForce(glm::vec3 velocity, const btCollisionObject *ground) {
   velocity.y = 0.0f;
   float speed = glm::length(velocity);
   float forceModifier = ground ? ground->getFriction() : AIR_MOVE_MULTIPLIER;
   float normalMoveForce = NORMAL_MOVE_FORCE * forceModifier;
   float maxMoveForce = MAX_MOVE_FORCE * forceModifier;
   return speed < normalMoveForce / maxMoveForce ? maxMoveForce : glm::min(maxMoveForce, normalMoveForce / speed);
}

} // namespace

PlayerLogicComponent::PlayerLogicComponent(GameObject &gameObject)
   : LogicComponent(gameObject), wasJumpingLastFrame(false) {
}

PlayerLogicComponent::~PlayerLogicComponent() {
}

void PlayerLogicComponent::tick(const float dt) {
   SPtr<Scene> scene = gameObject.getScene().lock();
   ASSERT(scene, "PlayerLogicComponent must be in Scene to tick");
   if (!scene) {
      return;
   }

   btCollisionObject* collisionObject = gameObject.getPhysicsComponent().getCollisionObject();
   btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(collisionObject);
   ASSERT(rigidBody, "Player rigid body should not be null");
   if (!rigidBody) {
      return;
   }

   const InputValues &inputValues = gameObject.getInputComponent().getInputValues();

   // Camera orientation
   CameraComponent &cameraComponent = gameObject.getCameraComponent();
   float lookAmount = dt * LOOK_SPEED;
   float pitchAmount = lookAmount * inputValues.lookY;
   float yawAmount = lookAmount * inputValues.lookX;
   glm::vec3 front = cameraComponent.getFrontVector();
   glm::vec3 right = cameraComponent.getRightVector();

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

   btVector3 velocity = rigidBody->getLinearVelocity();
   const btCollisionObject *ground = getGround(scene, *rigidBody);

   float forceAmount = calcMovementForce(toGlm(velocity), ground);

   glm::vec3 force = moveIntention * glm::vec3(forceAmount);

   rigidBody->applyCentralForce(toBt(force));

   // TODO Figure out hop bug
   if (ground && inputValues.jump && !wasJumpingLastFrame) {
      wasJumpingLastFrame = true;
      rigidBody->applyCentralForce(btVector3(0.0f, JUMP_FORCE, 0.0f));
   }

   if (!inputValues.jump) {
      wasJumpingLastFrame = false;
   }
}
