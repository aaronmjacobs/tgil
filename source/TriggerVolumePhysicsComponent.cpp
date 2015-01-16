#include "Conversions.h"
#include "GameObject.h"
#include "TriggerVolumePhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

TriggerVolumePhysicsComponent::TriggerVolumePhysicsComponent(GameObject &gameObject, glm::vec3 halfExtents, const CollisionGroup::Group mask)
   : PhysicsComponent(gameObject, btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE, CollisionGroup::Triggers, mask) {
   collisionShape = UPtr<btCollisionShape>(new btBoxShape(toBt(halfExtents)));

   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   btTransform transform(toBt(gameObject.getOrientation()), toBt(gameObject.getPosition()));

   UPtr<btGhostObject> ghostObject(new btGhostObject);
   ghostObject->setCollisionShape(collisionShape.get());
   ghostObject->setWorldTransform(transform);
   collisionObject = std::move(ghostObject);
}

TriggerVolumePhysicsComponent::~TriggerVolumePhysicsComponent() {
}
