#ifndef SCENE_H
#define SCENE_H

#include "Types.h"

#include <set>
#include <vector>

class DebugDrawer;
class GameObject;
class PhysicsManager;
class PlayerLogicComponent;
class ShaderProgram;

namespace {

const int NO_WINNER = -1;

}

class GameState {
protected:
   int winner;

public:
   GameState()
      : winner(NO_WINNER) {}

   bool hasWinner() const {
      return winner != NO_WINNER;
   }

   int getWinner() const {
      return winner;
   }

   void setWinner(int winner) {
      if (!hasWinner()) {
         this->winner = winner;
      }
   }
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

   void updateWinState();

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

   SPtr<GameObject> getPlayerByNumber(int playerNum) const;

   std::vector<SPtr<GameObject>> getLivingPlayers() const;
};

#endif
