#include "MeshPhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>

namespace {

glm::vec3 toGlm(btVector3 vec) {
   return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
}

glm::quat toGlm(btQuaternion quat) {
   return glm::quat(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
}

} // namespace

MeshPhysicsComponent::MeshPhysicsComponent(GameObject &gameObject, float mass)
   : PhysicsComponent(gameObject, mass) {
   // TODO get shape from mesh
   collisionShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));

   const glm::quat &orientation = gameObject.getOrientation();
   motionState = new btDefaultMotionState(btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w)));

   btVector3 fallInertia(0, 0, 0);
   collisionShape->calculateLocalInertia(mass, fallInertia);
   btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, collisionShape, fallInertia);

   rigidBody = new btRigidBody(constructionInfo);
}

MeshPhysicsComponent::~MeshPhysicsComponent() {
   // TODO Make sure the rigidBody is removed from the scene

   delete rigidBody;
   delete motionState;
   delete collisionShape;
}

void MeshPhysicsComponent::tick() {
   btTransform trans;
   rigidBody->getMotionState()->getWorldTransform(trans);

   gameObject.setPosition(toGlm(trans.getOrigin()));
   gameObject.setOrientation(toGlm(trans.getRotation()));
}
