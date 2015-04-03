#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GameObjectMotionState.h"
#include "PhysicsManager.h"
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
   : PhysicsComponent(gameObject, CollisionGroup::Characters, CollisionGroup::Everything) {
   collisionShape = UPtr<btCollisionShape>(new btCapsuleShape(PLAYER_RADIUS, PLAYER_MIDDLE_HEIGHT));

   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   motionState = UPtr<btMotionState>(new PlayerMotionState(gameObject));

   btVector3 inertia(0.0f, 0.0f, 0.0f);
   collisionShape->calculateLocalInertia(mass, inertia);
   btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState.get(), collisionShape.get(), inertia);

   UPtr<btRigidBody> rigidBody(new btRigidBody(constructionInfo));
   rigidBody->setAngularFactor(0.0f); // Prevent the capsule from falling over
   rigidBody->setSleepingThresholds(0.0f, 0.0f); // Prevent the capsule from sleeping

   collisionObject = std::move(rigidBody);
}

PlayerPhysicsComponent::~PlayerPhysicsComponent() {
}

glm::vec3 PlayerPhysicsComponent::getVelocity() const {
   btRigidBody *rigidBody = dynamic_cast<btRigidBody*>(getCollisionObject());
   if (!rigidBody) {
      return glm::vec3(0.0f);
   }

   return toGlm(rigidBody->getLinearVelocity());
}
