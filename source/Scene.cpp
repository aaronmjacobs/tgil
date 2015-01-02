#include "GameObject.h"
#include "Material.h"
#include "Model.h"
#include "PhysicsManager.h"
#include "Scene.h"

Scene::Scene()
   : physicsManager(new PhysicsManager) {
}

Scene::~Scene() {
}

void Scene::tick(const double dt) {
   for (SPtr<GameObject> object : objects) {
      // TODO Tick
   }
}

void Scene::addLight(SPtr<GameObject> light) {
   lights.push_back(light);
}

void Scene::addObject(SPtr<GameObject> object) {
   objects.push_back(object);

   SPtr<Model> model = object->getModel();
   if (model) {
      shaderPrograms.insert(model->getMaterial().getShaderProgram());
   }
}
