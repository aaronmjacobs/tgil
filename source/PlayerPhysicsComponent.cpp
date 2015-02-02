#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GameObjectMotionState.h"
#include "PhysicsManager.h"
#include "PlayerPhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

// Custom motion state

class PlayerMotionState : public GameObjectMotionState {
protected:
   WPtr<btGhostObject> ghostObject;

public:
   PlayerMotionState(GameObject &gameObject, WPtr<btGhostObject> ghostObject);

   virtual ~PlayerMotionState();

   virtual void getWorldTransform(btTransform &worldTrans) const;

   virtual void setWorldTransform(const btTransform &worldTrans);
};

PlayerMotionState::PlayerMotionState(GameObject &gameObject, WPtr<btGhostObject> ghostObject)
   : GameObjectMotionState(gameObject), ghostObject(ghostObject) {
}

PlayerMotionState::~PlayerMotionState() {
}

void PlayerMotionState::getWorldTransform(btTransform &worldTrans) const {
   worldTrans.setOrigin(toBt(gameObject.getPosition()));
   worldTrans.setRotation(toBt(glm::quat()));

   SPtr<btGhostObject> ghost = ghostObject.lock();
   if (ghost) {
      btTransform trans = worldTrans;
      trans.setOrigin(trans.getOrigin() + btVector3(0.0f, -PLAYER_GHOST_OFFSET, 0.0f));
      ghost->setWorldTransform(trans);
   }
}

void PlayerMotionState::setWorldTransform(const btTransform &worldTrans) {
   gameObject.setPosition(toGlm(worldTrans.getOrigin()));
   // Orientation is ignored, since the player is a capsule

   SPtr<btGhostObject> ghost = ghostObject.lock();
   if (ghost) {
      btTransform trans = worldTrans;
      trans.setOrigin(trans.getOrigin() + btVector3(0.0f, -PLAYER_GHOST_OFFSET, 0.0f));
      ghost->setWorldTransform(trans);
   }
}

// PlayerPhysicsComponent

PlayerPhysicsComponent::PlayerPhysicsComponent(GameObject &gameObject, float mass)
   : PhysicsComponent(gameObject, CollisionGroup::Characters, CollisionGroup::Everything) {
   collisionShape = UPtr<btCollisionShape>(new btCapsuleShape(PLAYER_RADIUS, PLAYER_MIDDLE_HEIGHT));

   ghostObject = std::make_shared<btGhostObject>();
   ghostCollisionShape = UPtr<btCollisionShape>(new btCapsuleShape(PLAYER_GHOST_RADIUS, PLAYER_GHOST_MIDDLE_HEIGHT));

   collisionShape->setLocalScaling(toBt(gameObject.getScale()));
   ghostCollisionShape->setLocalScaling(toBt(gameObject.getScale()));

   motionState = UPtr<btMotionState>(new PlayerMotionState(gameObject, ghostObject));

   btVector3 inertia(0.0f, 0.0f, 0.0f);
   collisionShape->calculateLocalInertia(mass, inertia);
   btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState.get(), collisionShape.get(), inertia);

   UPtr<btRigidBody> rigidBody(new btRigidBody(constructionInfo));
   rigidBody->setAngularFactor(0.0f); // Prevent the capsule from falling over
   rigidBody->setSleepingThresholds(0.0f, 0.0f); // Prevent the capsule from sleeping

   collisionObject = std::move(rigidBody);

   // Ghost object
   ghostObject->setCollisionShape(ghostCollisionShape.get());
   ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

PlayerPhysicsComponent::~PlayerPhysicsComponent() {
   // Do removals here (instead of PhysicsComponent) so that the ghost object still exists during removal time
   while (!physicsManagers.empty()) {
      WPtr<PhysicsManager> wManager = *physicsManagers.begin();
      SPtr<PhysicsManager> manager = wManager.lock();
      if (manager) {
         removeFromManager(manager);
      }
   }
}

void PlayerPhysicsComponent::addToManager(SPtr<PhysicsManager> manager) {
   ASSERT(manager, "Trying to add to null manager");

   PhysicsComponent::addToManager(manager);

   manager->getDynamicsWorld().addCollisionObject(ghostObject.get(), CollisionGroup::Ghosts, CollisionGroup::Everything);
}

void PlayerPhysicsComponent::removeFromManager(SPtr<PhysicsManager> manager) {
   ASSERT(manager, "Trying to remove from null manager");

   manager->getDynamicsWorld().removeCollisionObject(ghostObject.get());

   PhysicsComponent::removeFromManager(manager);
}

void PlayerPhysicsComponent::onNotify(const GameObject &gameObject, Event event) {
   PhysicsComponent::onNotify(gameObject, event);

   switch (event) {
      case SCALE: {
         ghostCollisionShape->setLocalScaling(toBt(gameObject.getScale()));
         break;
      }
      default:
         break;
   }
}

btGhostObject& PlayerPhysicsComponent::getGhostObject() const {
   ASSERT(ghostObject, "Ghost shouldn't be null");
   return *ghostObject;
}
