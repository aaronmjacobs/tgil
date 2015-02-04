#ifndef SCENE_H
#define SCENE_H

#include "Types.h"

#include <set>
#include <vector>

class DebugDrawer;
class GameObject;
class PhysicsManager;
class ShaderProgram;

struct GameState {
   int winner;

   GameState()
      : winner(-1) {}
};

struct GameObjectVectors {
   std::vector<SPtr<GameObject>> objects;
   std::vector<SPtr<GameObject>> toAdd;
   std::vector<SPtr<GameObject>> toRemove;
};

class Scene : public std::enable_shared_from_this<Scene> {
protected:
   GameState gameState;

   const SPtr<PhysicsManager> physicsManager;
   const UPtr<DebugDrawer> debugDrawer;

   std::set<SPtr<ShaderProgram>> shaderPrograms;

   GameObjectVectors players;
   GameObjectVectors cameras;
   GameObjectVectors lights;
   GameObjectVectors objects;

   bool ticking;

   bool addToVectors(GameObjectVectors &vectors, SPtr<GameObject> object);
   bool removeFromVectors(GameObjectVectors &vectors, SPtr<GameObject> object);
   void processPendingObjects();

public:
   Scene();

   virtual ~Scene();

   void tick(const float dt);

   const GameState& getGameState() const {
      return gameState;
   }

   void setWinner(int player);

   SPtr<PhysicsManager> getPhysicsManager() const {
      return physicsManager;
   }

   DebugDrawer& getDebugDrawer() {
      return *debugDrawer;
   }

   const std::vector<SPtr<GameObject>>& getPlayers() const {
      return players.objects;
   }

   const std::vector<SPtr<GameObject>>& getCameras() const {
      return cameras.objects;
   }

   const std::vector<SPtr<GameObject>>& getLights() const {
      return lights.objects;
   }

   const std::vector<SPtr<GameObject>>& getObjects() const {
      return objects.objects;
   }

   const std::set<SPtr<ShaderProgram>>& getShaderPrograms() const {
      return shaderPrograms;
   }

   void addPlayer(SPtr<GameObject> player);

   void addCamera(SPtr<GameObject> camera);

   void addLight(SPtr<GameObject> light);

   void addObject(SPtr<GameObject> object);

   void removePlayer(SPtr<GameObject> player);

   void removeCamera(SPtr<GameObject> camera);

   void removeLight(SPtr<GameObject> light);

   void removeObject(SPtr<GameObject> object);
};

#endif
