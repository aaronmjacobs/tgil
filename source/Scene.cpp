#include "GameObject.h"
#include "Material.h"
#include "Model.h"
#include "PhysicsComponent.h"
#include "PhysicsManager.h"
#include "Scene.h"

Scene::Scene()
   : physicsManager(std::make_shared<PhysicsManager>()) {
}

Scene::~Scene() {
}

void Scene::tick(const float dt) {
   physicsManager->tick(dt);

   for (SPtr<GameObject> object : objects) {
      object->tick(dt);
   }
}

void Scene::addLight(SPtr<GameObject> light) {
   lights.push_back(light);
}

void Scene::addObject(SPtr<GameObject> object) {
   PhysicsComponent &physicsComponent = object->getPhysicsComponent();
   if (physicsComponent.getRigidBody()) {
      physicsManager->addObject(physicsComponent);
   }

   objects.push_back(object);

   SPtr<Model> model = object->getModel();
   if (model) {
      shaderPrograms.insert(model->getMaterial().getShaderProgram());
   }
}
