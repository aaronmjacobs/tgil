#include "AudioManager.h"
#include "CameraComponent.h"
#include "Constants.h"
#include "Context.h"
#include "DebugDrawer.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "InputComponent.h"
#include "Material.h"
#include "Model.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "PlayerLogicComponent.h"
#include "PlayerPhysicsComponent.h"
#include "Scene.h"

#include <algorithm>

Scene::Scene()
   : ended(false), physicsManager(std::make_shared<PhysicsManager>()), debugDrawer(new DebugDrawer), ticking(false), timeSinceStart(0.0f), timeSinceEnd(0.0f) {
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

void Scene::updateWinState() {
   if (gameState.hasWinner()) {
      ended = true;
      return;
   }

   std::vector<SPtr<GameObject>> livingPlayers(getLivingPlayers());
   if (livingPlayers.size() != 1) {
      return;
   }

   PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&livingPlayers[0]->getLogicComponent());
   if (!playerLogic) {
      return;
   }

   int playerNumber = playerLogic->getPlayerNum();
   setWinner(playerNumber);
}

void Scene::updateAudio() {
   AudioManager &audioManager = Context::getInstance().getAudioManager();

   int numCameras = cameras.objects.size();
   UPtr<ListenerAttributes[]> attributes(new ListenerAttributes[numCameras]);

   for (int i = 0; i < numCameras; ++i) {
      CameraComponent &cameraComponent = cameras.objects[i]->getCameraComponent();
      PlayerPhysicsComponent *physicsComponent = dynamic_cast<PlayerPhysicsComponent*>(&cameras.objects[i]->getPhysicsComponent());

      glm::vec3 velocity(0.0f);
      if (physicsComponent) {
         velocity = physicsComponent->getVelocity();
      }

      attributes[i] = { cameraComponent.getCameraPosition(), velocity, cameraComponent.getFrontVector(), cameraComponent.getUpVector() };
   }

   audioManager.update(attributes.get(), numCameras);
}

void Scene::setWinner(int player) {
   ASSERT(player >= 0 && player < MAX_PLAYERS, "Invalid player index");
   gameState.setWinner(player);
}

void Scene::tick(const float dt) {
   updateAudio();

   processPendingObjects();

   ticking = true;

   physicsManager->tick(dt);

   for (SPtr<GameObject> object : objects.objects) {
      object->tick(dt);
   }

   updateWinState();

   timeSinceStart += dt;
   if (ended) {
      timeSinceEnd += dt;
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

SPtr<GameObject> Scene::getPlayerByNumber(int playerNum) const {
   for (SPtr<GameObject> player : players.objects) {
      PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&player->getLogicComponent());
      if (playerLogic && playerNum == playerLogic->getPlayerNum()) {
         return player;
      }
   }

   return nullptr;
}

std::vector<SPtr<GameObject>> Scene::getLivingPlayers() const {
   std::vector<SPtr<GameObject>> livingPlayers;

   for (SPtr<GameObject> player : players.objects) {
      PlayerLogicComponent *playerLogic = dynamic_cast<PlayerLogicComponent*>(&player->getLogicComponent());
      ASSERT(playerLogic, "Player should have PlayerLogicComponent");

      if (playerLogic && playerLogic->isAlive()) {
         livingPlayers.push_back(player);
      }
   }

   return livingPlayers;
}

void Scene::addClickableObject(ClickableObject clickableObject) {
   clickableObjects.push_back(clickableObject);
}

const std::vector<ClickableObject>& Scene::getClickableObjecst() const {
   return clickableObjects;
}
