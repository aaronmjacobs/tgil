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

void Scene::processPendingObjects() {
   ASSERT(!ticking, "Trying to process pending objects during tick (can cause concurrent modification issues)");

   for (SPtr<GameObject> camera : camerasToRemove) {
      removeCamera(camera);
   }
   camerasToRemove.clear();

   for (SPtr<GameObject> light : lightsToRemove) {
      removeLight(light);
   }
   lightsToRemove.clear();

   for (SPtr<GameObject> object : objectsToRemove) {
      removeObject(object);
   }
   objectsToRemove.clear();

   for (SPtr<GameObject> camera : camerasToAdd) {
      addCamera(camera);
   }
   camerasToAdd.clear();

   for (SPtr<GameObject> light : lightsToAdd) {
      addLight(light);
   }
   lightsToAdd.clear();

   for (SPtr<GameObject> object : objectsToAdd) {
      addObject(object);
   }
   objectsToAdd.clear();
}

void Scene::setWinner(int player) {
   ASSERT(player >= 0 && player < MAX_PLAYERS, "Invalid player index");
   gameState.winner = player;
}

void Scene::tick(const float dt) {
   processPendingObjects();

   ticking = true;

   physicsManager->tick(dt);

   for (SPtr<GameObject> object : objects) {
      object->tick(dt);
   }

   ticking = false;
}

void Scene::addCamera(SPtr<GameObject> camera) {
   ASSERT(camera, "Trying to add null camera to scene");

   if (ticking) {
      camerasToAdd.push_back(camera);
      return;
   }

   cameras.push_back(camera);
   addObject(camera);
}

void Scene::addLight(SPtr<GameObject> light) {
   ASSERT(light, "Trying to add null light in scene");

   if (ticking) {
      lightsToAdd.push_back(light);
      return;
   }

   lights.push_back(light);
   addObject(light);
}

void Scene::addObject(SPtr<GameObject> object) {
   ASSERT(object, "Trying to add null object in scene");

   if (ticking) {
      objectsToAdd.push_back(object);
      return;
   }

   object->getPhysicsComponent().addToManager(physicsManager);

   objects.push_back(object);

   SPtr<Model> model = object->getGraphicsComponent().getModel();
   if (model) {
      shaderPrograms.insert(model->getShaderProgram());
   }

   object->setScene(shared_from_this());
}

void Scene::removeCamera(SPtr<GameObject> camera) {
   ASSERT(camera, "Trying to remove null camera from scene");

   if (ticking) {
      camerasToRemove.push_back(camera);
      return;
   }

   cameras.erase(std::remove(cameras.begin(), cameras.end(), camera), cameras.end());
   removeObject(camera);
}

void Scene::removeLight(SPtr<GameObject> light) {
   ASSERT(light, "Trying to remove null light from scene");

   if (ticking) {
      lightsToRemove.push_back(light);
      return;
   }

   lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
   removeObject(light);
}

void Scene::removeObject(SPtr<GameObject> object) {
   ASSERT(object, "Trying to remove null object from scene");

   if (ticking) {
      objectsToRemove.push_back(object);
      return;
   }

   object->getPhysicsComponent().removeFromManager(physicsManager);

   objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());

   /*SPtr<Model> model = object->getGraphicsComponent().getModel();
   if (model) {
      shaderPrograms.erase(model->getShaderProgram());
   }*/

   object->setScene(WPtr<Scene>());
}
