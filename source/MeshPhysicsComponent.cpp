#include "GameObject.h"
#include "MeshPhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

namespace {

glm::vec3 toGlm(btVector3 vec) {
   return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
}

glm::quat toGlm(btQuaternion quat) {
   return glm::quat(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
}

} // namespace

MeshPhysicsComponent::MeshPhysicsComponent(SPtr<GameObject> gameObject, float mass)
   : PhysicsComponent(gameObject, mass) {
   // TODO get shape from mesh
   collisionShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));

   const glm::quat &orientation = gameObject->getOrientation();
   const glm::vec3 &position = gameObject->getPosition();
   motionState = new btDefaultMotionState(btTransform(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w), btVector3(position.x, position.y, position.z)));

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

void MeshPhysicsComponent::tick(GameObject &gameObject) {
   btTransform trans;
   rigidBody->getMotionState()->getWorldTransform(trans);

   gameObject.setPosition(toGlm(trans.getOrigin()));
   gameObject.setOrientation(toGlm(trans.getRotation()));
}
