#ifndef SKY_RENDERER_H
#define SKY_RENDERER_H

#include "Types.h"

#include <glm/glm.hpp>

class GameObject;
class Model;
struct Viewport;

class SkyRenderer {
protected:
   UPtr<Model> xyPlane;

   void loadPlane();

public:
   SkyRenderer();

   virtual ~SkyRenderer();

   void init();

   void render(const glm::mat4 &view, const glm::mat4 &proj, const Viewport &viewport, const glm::vec2 &framebufferResolution, SPtr<GameObject> sun);
};

#endif
