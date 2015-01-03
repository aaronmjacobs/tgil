#include "GameObject.h"
#include "Mesh.h"
#include "Model.h"
#include "MeshPhysicsComponent.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include <glm/glm.hpp>

namespace {

glm::vec3 toGlm(const btVector3 &vec) {
   return glm::vec3(vec.x(), vec.y(), vec.z());
}

glm::quat toGlm(const btQuaternion &quat) {
   return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

btVector3 toBt(const glm::vec3 &vec) {
   return btVector3(vec.x, vec.y, vec.z);
}

btQuaternion toBt(const glm::quat &quat) {
   return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

} // namespace

MeshPhysicsComponent::MeshPhysicsComponent(GameObject &gameObject, float mass) {
   SPtr<Model> model = gameObject.getModel();
   if (model) {
      const Mesh &mesh = model->getMesh();
      collisionShape = UPtr<btCollisionShape>(new btConvexHullShape(mesh.getVertices(), mesh.getNumVertices(), sizeof(float) * 3));

      // TODO Handle mesh simplification
   } else {
      // If there is no model, use a unit cube as the default collision shape
      collisionShape = UPtr<btCollisionShape>(new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)));
   }

   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   motionState = UPtr<btMotionState>(new btDefaultMotionState(btTransform(toBt(gameObject.getOrientation()), toBt(gameObject.getPosition()))));

   btVector3 fallInertia(0, 0, 0);
   collisionShape->calculateLocalInertia(mass, fallInertia);
   btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState.get(), collisionShape.get(), fallInertia);

   rigidBody = UPtr<btRigidBody>(new btRigidBody(constructionInfo));
}

MeshPhysicsComponent::~MeshPhysicsComponent() {
   // TODO Figure out if the rigid body needs to be removed from the scene

   // Clean up in reverse order
   rigidBody.reset();
   motionState.reset();
   collisionShape.reset();
}

void MeshPhysicsComponent::tick(GameObject &gameObject) {
   btTransform trans;
   rigidBody->getMotionState()->getWorldTransform(trans);

   gameObject.setPosition(toGlm(trans.getOrigin()));
   gameObject.setOrientation(toGlm(trans.getRotation()));
}
