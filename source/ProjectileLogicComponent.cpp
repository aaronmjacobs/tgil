#include "GameObject.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "ProjectileLogicComponent.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>

namespace {

struct ProjectileSensorCallback : public btCollisionWorld::ContactResultCallback {
   btCollisionObject *collisionObject;
   bool collided = false;
   const btCollisionObject *objectCollidedWith;

   virtual btScalar addSingleResult(btManifoldPoint& cp,
                                    const btCollisionObjectWrapper* colObj0, int partId0, int index0,
                                    const btCollisionObjectWrapper* colObj1, int partId1, int index1) {
      collided = true;
      if (colObj0->getCollisionObject() == collisionObject) {
         objectCollidedWith = colObj1->getCollisionObject();
      } else {
         objectCollidedWith = colObj0->getCollisionObject();
      }

      return 0;
   }

   };

} // namespace

ProjectileLogicComponent::ProjectileLogicComponent(GameObject &gameObject)
   : LogicComponent(gameObject), lifeTime(0.0f) {
}

ProjectileLogicComponent::~ProjectileLogicComponent() {
}

void ProjectileLogicComponent::tick(const float dt) {
   lifeTime += dt;

   btCollisionObject *collisionObject = gameObject.getPhysicsComponent().getCollisionObject();
   if (!collisionObject) {
      return;
   }

   SPtr<Scene> scene = gameObject.getScene().lock();
   if (!scene) {
      return;
   }

   ProjectileSensorCallback callback;
   callback.collisionObject = collisionObject;
   scene->getPhysicsManager()->getDynamicsWorld().contactTest(collisionObject, callback);

   if (callback.collided && collisionCallback) {
      collisionCallback(gameObject, callback.objectCollidedWith, dt);
   }
}
