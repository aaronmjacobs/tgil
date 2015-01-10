#include "Conversions.h"
#include "GameObject.h"
#include "PlayerPhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>

PlayerPhysicsComponent::PlayerPhysicsComponent(GameObject &gameObject, float mass)
   : PhysicsComponent(gameObject) {
   collisionShape = UPtr<btCollisionShape>(new btCapsuleShape(0.25f, 1.0f));
   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   motionState = UPtr<btMotionState>(new btDefaultMotionState(btTransform(toBt(gameObject.getOrientation()), toBt(gameObject.getPosition()))));

   btVector3 fallInertia(0, 0, 0);
   collisionShape->calculateLocalInertia(mass, fallInertia);
   btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState.get(), collisionShape.get(), fallInertia);

   rigidBody = UPtr<btRigidBody>(new btRigidBody(constructionInfo));
   rigidBody->setAngularFactor(0.0f); // Prevent the capsule from falling over
   rigidBody->setSleepingThresholds(0.0f, 0.0f); // Prevent the capsule from sleeping
}

PlayerPhysicsComponent::~PlayerPhysicsComponent() {
}

void PlayerPhysicsComponent::tick() {
   btTransform trans;
   rigidBody->getMotionState()->getWorldTransform(trans);

   gameObject.setPosition(toGlm(trans.getOrigin()));
   // Orientation is ignored, since the player is a capsule
}
