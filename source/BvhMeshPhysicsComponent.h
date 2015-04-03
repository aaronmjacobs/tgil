#ifndef BVH_MESH_PHYSICS_COMPONENT_H
#define BVH_MESH_PHYSICS_COMPONENT_H

#include "PhysicsComponent.h"

class btTriangleIndexVertexArray;

class BvhMeshPhysicsComponent : public PhysicsComponent {
protected:
   UPtr<btTriangleIndexVertexArray> triangles;

public:
   BvhMeshPhysicsComponent(GameObject &gameObject, const CollisionGroup::Group collisionGroup, const short collisionMask);

   virtual ~BvhMeshPhysicsComponent();
};

#endif
