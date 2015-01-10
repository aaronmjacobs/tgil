#include "Conversions.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Model.h"
#include "MeshPhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include <glm/glm.hpp>

MeshPhysicsComponent::MeshPhysicsComponent(GameObject &gameObject, float mass)
   : PhysicsComponent(gameObject) {
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
}

void MeshPhysicsComponent::tick() {
   btTransform trans;
   rigidBody->getMotionState()->getWorldTransform(trans);

   gameObject.setPosition(toGlm(trans.getOrigin()));
   gameObject.setOrientation(toGlm(trans.getRotation()));
}
