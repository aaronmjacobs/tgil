#include "Constants.h"
#include "DebugDrawer.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "Material.h"
#include "Model.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

#include <algorithm>

Scene::Scene()
   : physicsManager(std::make_shared<PhysicsManager>()), debugDrawer(new DebugDrawer), ticking(false) {
   physicsManager->setDebugDrawer(debugDrawer.get());
}

Scene::~Scene() {
}

bool Scene::addToVectors(GameObjectVectors &vectors, SPtr<GameObject> object) {
   ASSERT(object, "Trying to add null object to scene");

   if (ticking) {
      vectors.toAdd.push_back(object);
      return false;
   }

   vectors.objects.push_back(object);
   return true;
}

bool Scene::removeFromVectors(GameObjectVectors &vectors, SPtr<GameObject> object) {
   ASSERT(object, "Trying to remove null object from scene");

   if (ticking) {
      vectors.toRemove.push_back(object);
      return false;
   }

   vectors.objects.erase(std::remove(vectors.objects.begin(), vectors.objects.end(), object), vectors.objects.end());
   return true;
}

void Scene::processPendingObjects() {
   ASSERT(!ticking, "Trying to process pending objects during tick (can cause concurrent modification issues)");

   // Removals

   for (SPtr<GameObject> player : players.toRemove) {
      removePlayer(player);
   }
   players.toRemove.clear();

   for (SPtr<GameObject> camera : cameras.toRemove) {
      removeCamera(camera);
   }
   cameras.toRemove.clear();

   for (SPtr<GameObject> light : lights.toRemove) {
      removeLight(light);
   }
   lights.toRemove.clear();

   for (SPtr<GameObject> object : objects.toRemove) {
      removeObject(object);
   }
   objects.toRemove.clear();

   // Additions

   for (SPtr<GameObject> player : players.toAdd) {
      addPlayer(player);
   }
   players.toAdd.clear();

   for (SPtr<GameObject> camera : cameras.toAdd) {
      addCamera(camera);
   }
   cameras.toAdd.clear();

   for (SPtr<GameObject> light : lights.toAdd) {
      addLight(light);
   }
   lights.toAdd.clear();

   for (SPtr<GameObject> object : objects.toAdd) {
      addObject(object);
   }
   objects.toAdd.clear();
}

void Scene::setWinner(int player) {
   ASSERT(player >= 0 && player < MAX_PLAYERS, "Invalid player index");
   gameState.winner = player;
}

void Scene::tick(const float dt) {
   processPendingObjects();

   ticking = true;

   physicsManager->tick(dt);

   for (SPtr<GameObject> object : objects.objects) {
      object->tick(dt);
   }

   ticking = false;
}

void Scene::addPlayer(SPtr<GameObject> player) {
   addToVectors(players, player);
}

void Scene::addCamera(SPtr<GameObject> camera) {
   addToVectors(cameras, camera);
}

void Scene::addLight(SPtr<GameObject> light) {
   addToVectors(lights, light);
}

void Scene::addObject(SPtr<GameObject> object) {
   bool addedNow = addToVectors(objects, object);
   if (!addedNow) {
      return;
   }

   object->getPhysicsComponent().addToManager(physicsManager);

   SPtr<Model> model = object->getGraphicsComponent().getModel();
   if (model) {
      shaderPrograms.insert(model->getShaderProgram());
   }

   object->setScene(shared_from_this());
}

void Scene::removePlayer(SPtr<GameObject> player) {
   removeFromVectors(players, player);
}

void Scene::removeCamera(SPtr<GameObject> camera) {
   removeFromVectors(cameras, camera);
}

void Scene::removeLight(SPtr<GameObject> light) {
   removeFromVectors(lights, light);
}

void Scene::removeObject(SPtr<GameObject> object) {
   bool removedNow = removeFromVectors(objects, object);
   if (!removedNow) {
      return;
   }

   object->getPhysicsComponent().removeFromManager(physicsManager);

   // TODO Make shared programs a set of WPtrs?
   // (auto-clean on each tick)
   /*SPtr<Model> model = object->getGraphicsComponent().getModel();
   if (model) {
      shaderPrograms.erase(model->getShaderProgram());
   }*/

   object->setScene(WPtr<Scene>());
}
