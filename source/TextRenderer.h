#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "GLIncludes.h"

class DynamicMesh;
class FontAtlas;
class GameObject;
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

class TextRenderer {
protected:
   SPtr<DynamicMesh> mesh;
   SPtr<TextureMaterial> textureMaterial;
   SPtr<GameObject> gameObject;
   UPtr<FontAtlas> atlas;
   bool initialized;

   void loadFontAtlases(float pixelDensity);

public:
   TextRenderer();

   virtual ~TextRenderer();

   void init(float pixelDensity);

   void onPixelDensityChange(float pixelDensity);

   void render(int fbWidth, int fbHeight, float x, float y, const std::string &text, FontType fontType = FontType::Medium, HAlign hAlign = HAlign::Center, VAlign vAlign = VAlign::Center);
};

#endif
