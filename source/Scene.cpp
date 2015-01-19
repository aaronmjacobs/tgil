#include "DebugDrawer.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "GraphicsComponent.h"
#include "Material.h"
#include "Model.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

Scene::Scene()
   : physicsManager(std::make_shared<PhysicsManager>()), debugDrawer(new DebugDrawer), ticking(false) {
   physicsManager->setDebugDrawer(debugDrawer.get());
}

Scene::~Scene() {
}

void Scene::addPendingObjects() {
   ASSERT(!ticking, "Trying to add pending objects during tick (can cause concurrent modification issues)");

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

void Scene::tick(const float dt) {
   addPendingObjects();

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

   physicsManager->addObject(object->getPhysicsComponent());

   objects.push_back(object);

   SPtr<Model> model = object->getGraphicsComponent().getModel();
   if (model) {
      shaderPrograms.insert(model->getShaderProgram());
   }

   object->setScene(shared_from_this());
}
