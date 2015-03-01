#ifndef POST_PROCESS_RENDERER_H
#define POST_PROCESS_RENDERER_H

#include "Types.h"

#include <glm/glm.hpp>

class Model;
class TintMaterial;

class PostProcessRenderer {
protected:
   UPtr<Model> xyPlane;
   SPtr<TintMaterial> material;

   void loadPlane();

public:
   PostProcessRenderer();

   virtual ~PostProcessRenderer();

   void init();

   void render(float opacity, const glm::vec3 &tint);
};

#endif
