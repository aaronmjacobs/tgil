#include "BvhMeshPhysicsComponent.h"
#include "Conversions.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "Mesh.h"
#include "Model.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <bullet/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

BvhMeshPhysicsComponent::BvhMeshPhysicsComponent(GameObject &gameObject, const CollisionGroup::Group collisionGroup, const short collisionMask)
   : PhysicsComponent(gameObject, collisionGroup, collisionMask) {
   SPtr<Model> model = gameObject.getGraphicsComponent().getModel();
   ASSERT(model, "Must have model to construct BvhMeshPhysicsComponent");

   const Mesh &mesh = model->getMesh();

   triangles = UPtr<btTriangleIndexVertexArray>(new btTriangleIndexVertexArray(mesh.getNumIndices() / 3, (int*)mesh.getIndices(), 3 * sizeof(unsigned int), mesh.getNumVertices(), mesh.getVertices(), 3 * sizeof(float)));

   collisionShape = UPtr<btCollisionShape>(new btBvhTriangleMeshShape(triangles.get(), true, true));

   collisionShape->setLocalScaling(toBt(gameObject.getScale()));

   collisionObject = UPtr<btCollisionObject>(new btCollisionObject);

   btTransform transform;
   transform.setIdentity();
   transform.setOrigin(toBt(gameObject.getPosition()));
   collisionObject->setWorldTransform(transform);
   collisionObject->setCollisionShape(collisionShape.get());
}

BvhMeshPhysicsComponent::~BvhMeshPhysicsComponent() {
}
