#include "Conversions.h"
#include "GameObject.h"
#include "GhostPhysicsComponent.h"
#include "GraphicsComponent.h"
#include "Mesh.h"
#include "Model.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

GhostPhysicsComponent::GhostPhysicsComponent(GameObject &gameObject, const int mask, const bool dynamic)
   : PhysicsComponent(gameObject, (dynamic ? 0 : btCollisionObject::CF_STATIC_OBJECT) | btCollisionObject::CF_NO_CONTACT_RESPONSE, CollisionGroup::Ghosts, mask) {
   SPtr<Model> model = gameObject.getGraphicsComponent().getModel();
   if (model) {
      const Mesh &mesh = model->getMesh();
      collisionShape = UPtr<btCollisionShape>(new btConvexHullShape(mesh.getVertices(), mesh.getNumVertices(), sizeof(float) * 3));

      // TODO Handle mesh simplification
   } else {
      // If there is no model, use a unit sphere as the default collision shape
      collisionShape = UPtr<btCollisionShape>(new btSphereShape(0.5f));
   }
   init();
}

GhostPhysicsComponent::GhostPhysicsComponent(GameObject &gameObject, const int mask, const bool dynamic, const float radius)
   : PhysicsComponent(gameObject, (dynamic ? 0 : btCollisionObject::CF_STATIC_OBJECT) | btCollisionObject::CF_NO_CONTACT_RESPONSE, CollisionGroup::Ghosts, mask) {
   collisionShape = UPtr<btCollisionShape>(new btSphereShape(radius));
   init();
}

GhostPhysicsComponent::GhostPhysicsComponent(GameObject &gameObject, const int mask, const bool dynamic, const glm::vec3 &halfExtents)
   : PhysicsComponent(gameObject, (dynamic ? 0 : btCollisionObject::CF_STATIC_OBJECT) | btCollisionObject::CF_NO_CONTACT_RESPONSE, CollisionGroup::Ghosts, mask) {
   collisionShape = UPtr<btCollisionShape>(new btBoxShape(toBt(halfExtents)));
   init();
}

GhostPhysicsComponent::~GhostPhysicsComponent() {
}

void GhostPhysicsComponent::init() {
   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   btTransform transform(toBt(gameObject.getOrientation()), toBt(gameObject.getPosition()));

   UPtr<btGhostObject> ghostObject(new btGhostObject);
   ghostObject->setCollisionShape(collisionShape.get());
   ghostObject->setWorldTransform(transform);
   collisionObject = std::move(ghostObject);
}
