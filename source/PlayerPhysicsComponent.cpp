#include "Conversions.h"
#include "GameObject.h"
#include "GameObjectMotionState.h"
#include "PlayerPhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>

// Custom motion state

class PlayerMotionState : public GameObjectMotionState {
public:
   PlayerMotionState(GameObject &gameObject);

   virtual ~PlayerMotionState();

   virtual void getWorldTransform(btTransform &worldTrans) const;

   virtual void setWorldTransform(const btTransform &worldTrans);
};

PlayerMotionState::PlayerMotionState(GameObject &gameObject)
   : GameObjectMotionState(gameObject) {
}

PlayerMotionState::~PlayerMotionState() {
}

void PlayerMotionState::getWorldTransform(btTransform &worldTrans) const {
   worldTrans.setOrigin(toBt(gameObject.getPosition()));
   worldTrans.setRotation(toBt(glm::quat()));
}

void PlayerMotionState::setWorldTransform(const btTransform &worldTrans) {
   gameObject.setPosition(toGlm(worldTrans.getOrigin()));
   // Orientation is ignored, since the player is a capsule
}

// PlayerPhysicsComponent

PlayerPhysicsComponent::PlayerPhysicsComponent(GameObject &gameObject, float mass)
   : PhysicsComponent(gameObject) {
   collisionShape = UPtr<btCollisionShape>(new btCapsuleShape(0.25f, 1.0f));
   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   motionState = UPtr<btMotionState>(new PlayerMotionState(gameObject));

   btVector3 fallInertia(0, 0, 0);
   collisionShape->calculateLocalInertia(mass, fallInertia);
   btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState.get(), collisionShape.get(), fallInertia);

   rigidBody = UPtr<btRigidBody>(new btRigidBody(constructionInfo));
   rigidBody->setAngularFactor(0.0f); // Prevent the capsule from falling over
   rigidBody->setSleepingThresholds(0.0f, 0.0f); // Prevent the capsule from sleeping
}

PlayerPhysicsComponent::~PlayerPhysicsComponent() {
}
