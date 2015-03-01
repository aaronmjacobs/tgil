#ifndef HUD_RENDERER_H
#define HUD_RENDERER_H

#include "GLIncludes.h"
#include "Types.h"

#include <vector>

class Model;
class TextureMaterial;

struct HUDElement {
   GLuint textureID;
   float x;
   float y;
   float scale;
};

class HUDRenderer {
protected:
   UPtr<Model> xyPlane;

   SPtr<TextureMaterial> textureMaterial;

   GLint uTransform;

   std::vector<HUDElement> elements;

   void loadPlane();

public:
   HUDRenderer();

   virtual ~HUDRenderer();

   void init();

   void attach(const HUDElement &element);

   void render(int width, int height);
};

#endif
