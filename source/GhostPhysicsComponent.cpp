#include "Conversions.h"
#include "GameObject.h"
#include "GhostPhysicsComponent.h"
#include "GraphicsComponent.h"
#include "Mesh.h"
#include "Model.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

GhostPhysicsComponent::GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short collisionMask)
   : PhysicsComponent(gameObject, CollisionGroup::Ghosts, collisionMask) {
   SPtr<Model> model = gameObject.getGraphicsComponent().getModel();
   if (model) {
      const Mesh &mesh = model->getMesh();
      collisionShape = UPtr<btCollisionShape>(new btConvexHullShape(mesh.getVertices(), mesh.getNumVertices(), sizeof(float) * 3));

      // TODO Handle mesh simplification
   } else {
      // If there is no model, use a unit sphere as the default collision shape
      collisionShape = UPtr<btCollisionShape>(new btSphereShape(0.5f));
   }
   initCollisionObject(dynamic);
}

GhostPhysicsComponent::GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short collisionMask, const float radius)
   : PhysicsComponent(gameObject, CollisionGroup::Ghosts, collisionMask) {
   collisionShape = UPtr<btCollisionShape>(new btSphereShape(radius));
   initCollisionObject(dynamic);
}

GhostPhysicsComponent::GhostPhysicsComponent(GameObject &gameObject, const bool dynamic, const short collisionMask, const glm::vec3 &halfExtents)
   : PhysicsComponent(gameObject, CollisionGroup::Ghosts, collisionMask) {
   collisionShape = UPtr<btCollisionShape>(new btBoxShape(toBt(halfExtents)));
   initCollisionObject(dynamic);
}

GhostPhysicsComponent::~GhostPhysicsComponent() {
}

void GhostPhysicsComponent::initCollisionObject(const bool dynamic) {
   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   btTransform transform(toBt(gameObject.getOrientation()), toBt(gameObject.getPosition()));

   UPtr<btGhostObject> ghostObject(new btGhostObject);
   ghostObject->setCollisionShape(collisionShape.get());
   ghostObject->setWorldTransform(transform);

   int flags = btCollisionObject::CF_NO_CONTACT_RESPONSE;
   if (!dynamic) {
      flags |= btCollisionObject::CF_STATIC_OBJECT;
   }
   ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | flags);

   collisionObject = std::move(ghostObject);
}
