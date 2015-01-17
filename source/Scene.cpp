#include "DebugDrawer.h"
#include "FancyAssert.h"
#include "GameObject.h"
#include "Material.h"
#include "Model.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

Scene::Scene()
   : physicsManager(std::make_shared<PhysicsManager>()), debugDrawer(new DebugDrawer) {
   physicsManager->setDebugDrawer(debugDrawer.get());
}

Scene::~Scene() {
}

void Scene::tick(const float dt) {
   physicsManager->tick(dt);

   for (SPtr<GameObject> object : objects) {
      object->tick(dt);
   }
}

void Scene::setCamera(SPtr<GameObject> camera) {
   ASSERT(camera, "Trying to set null camera in scene");
   this->camera = camera;
   addObject(camera);
}

void Scene::addLight(SPtr<GameObject> light) {
   ASSERT(light, "Trying to add null light in scene");
   lights.push_back(light);
   addObject(light);
}

void Scene::addObject(SPtr<GameObject> object) {
   ASSERT(object, "Trying to add null object in scene");
   physicsManager->addObject(object->getPhysicsComponent());

   objects.push_back(object);

   SPtr<Model> model = object->getModel();
   if (model) {
      shaderPrograms.insert(model->getShaderProgram());
   }

   object->setScene(shared_from_this());
}
