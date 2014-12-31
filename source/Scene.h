#ifndef SCENE_H
#define SCENE_H

#include "Types.h"

#include <vector>

class GameObject;

class Scene {
protected:
   SPtr<GameObject> camera;
   std::vector<SPtr<GameObject>> lights;
   std::vector<SPtr<GameObject>> objects;

public:
   Scene();

   virtual ~Scene();

   void tick(const double dt);

   SPtr<GameObject> getCamera() const {
      return camera;
   }

   const std::vector<SPtr<GameObject>>& getLights() const {
      return lights;
   }

   const std::vector<SPtr<GameObject>>& getObjects() const {
      return objects;
   }

   void setCamera(SPtr<GameObject> camera) {
      this->camera = camera;
   }

   void addLight(SPtr<GameObject> light);

   void addObject(SPtr<GameObject> object);
};

#endif
