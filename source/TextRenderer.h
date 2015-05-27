#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "GLIncludes.h"

class DynamicMesh;
class FontAtlas;
class Framebuffer;
class GameObject;
class Texture;
class TextureMaterial;

enum class HAlign {
   Left,
   Center,
   Right
};

enum class VAlign {
   Top,
   Center,
   Bottom
};

enum class FontType {
   Small,
   Medium,
   LargeNumber
};

struct Resolution {
   float width;
   float height;

   Resolution(float width, float height)
      : width(width), height(height) {
   }
};

class TextRenderer {
protected:
   SPtr<DynamicMesh> mesh;
   SPtr<TextureMaterial> textureMaterial;
   SPtr<GameObject> gameObject;
   UPtr<FontAtlas> atlas;
   UPtr<Framebuffer> framebuffer;
   float pixelDensity;
   float targetPixelDensity;

   void loadFontAtlas(float pixelDensity);

public:
   TextRenderer();

   virtual ~TextRenderer();

   void init(float pixelDensity);

   void onPixelDensityChange(float pixelDensity);

   void renderImmediate(int fbWidth, int fbHeight, float x, float y, const std::string &text, FontType fontType = FontType::Medium, HAlign hAlign = HAlign::Center, VAlign vAlign = VAlign::Center);

   SPtr<Texture> renderToTexture(const std::string &text, Resolution *resolution, FontType fontType = FontType::Medium);
};

#endif
