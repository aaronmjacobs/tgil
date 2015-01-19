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

   std::set<SPtr<ShaderProgram>> shaderPrograms;

   std::vector<SPtr<GameObject>> cameras;
   std::vector<SPtr<GameObject>> lights;
   std::vector<SPtr<GameObject>> objects;

   bool ticking;
   std::vector<SPtr<GameObject>> camerasToAdd;
   std::vector<SPtr<GameObject>> lightsToAdd;
   std::vector<SPtr<GameObject>> objectsToAdd;
   std::vector<SPtr<GameObject>> camerasToRemove;
   std::vector<SPtr<GameObject>> lightsToRemove;
   std::vector<SPtr<GameObject>> objectsToRemove;

   void processPendingObjects();

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

   const std::vector<SPtr<GameObject>> getCameras() const {
      return cameras;
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

   void addCamera(SPtr<GameObject> camera);

   void addLight(SPtr<GameObject> light);

   void addObject(SPtr<GameObject> object);

   void removeCamera(SPtr<GameObject> camera);

   void removeLight(SPtr<GameObject> light);

   void removeObject(SPtr<GameObject> object);
};

#endif
