#ifndef SCENE_H
#define SCENE_H

#include "Types.h"

#include <set>
#include <vector>

class DebugDrawer;
class GameObject;
class PhysicsManager;
class ShaderProgram;

class Scene : public std::enable_shared_from_this<Scene> {
protected:
   const SPtr<PhysicsManager> physicsManager;
   const UPtr<DebugDrawer> debugDrawer;
   SPtr<GameObject> camera;
   std::vector<SPtr<GameObject>> lights;
   std::vector<SPtr<GameObject>> objects;
   std::set<SPtr<ShaderProgram>> shaderPrograms;

public:
   Scene();

   virtual ~Scene();

   void tick(const float dt);

   SPtr<PhysicsManager> getPhysicsManager() const {
      return physicsManager;
   }

   DebugDrawer& getDebugDrawer() {
      return *debugDrawer;
   }

   SPtr<GameObject> getCamera() const {
      return camera;
   }

   const std::vector<SPtr<GameObject>>& getLights() const {
      return lights;
   }

   const std::vector<SPtr<GameObject>>& getObjects() const {
      return objects;
   }

   const std::set<SPtr<ShaderProgram>>& getShaderPrograms() const {
      return shaderPrograms;
   }

   void setCamera(SPtr<GameObject> camera);

   SPtr<GameObject> getCamera() {
      return camera;
   }

   void addLight(SPtr<GameObject> light);

   void addObject(SPtr<GameObject> object);
};

#endif
