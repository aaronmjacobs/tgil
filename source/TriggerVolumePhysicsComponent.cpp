#include "Conversions.h"
#include "GameObject.h"
#include "TriggerVolumePhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>

TriggerVolumePhysicsComponent::TriggerVolumePhysicsComponent(GameObject &gameObject, glm::vec3 halfExtents, const CollisionGroup::Group mask)
   : PhysicsComponent(gameObject, CollisionType::Static, CollisionGroup::Triggers, mask) {
   collisionShape = UPtr<btCollisionShape>(new btBoxShape(toBt(halfExtents)));

   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   motionState = UPtr<btMotionState>(new btDefaultMotionState);
   btTransform transform(toBt(gameObject.getOrientation()), toBt(gameObject.getPosition()));
   motionState->setWorldTransform(transform);

   float mass = 0.0f;
   btVector3 inertia(0.0f, 0.0f, 0.0f);
   collisionShape->calculateLocalInertia(mass, inertia);
   btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState.get(), collisionShape.get(), inertia);

   // TODO Ghost object
   //collisionObject = UPtr<btCollisionObject>(new btCollisionObject(constructionInfo));
   //collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE | btCollisionObject::CF_STATIC_OBJECT);
}

TriggerVolumePhysicsComponent::~TriggerVolumePhysicsComponent() {
}
