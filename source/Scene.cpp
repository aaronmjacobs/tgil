#include "GameObject.h"
#include "Scene.h"

Scene::Scene() {
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
}
