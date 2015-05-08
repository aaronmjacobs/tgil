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
   Large,
   Number
};

class TextRenderer {
protected:
   SPtr<DynamicMesh> mesh;
   SPtr<TextureMaterial> textureMaterial;
   SPtr<GameObject> gameObject;
   UPtr<FontAtlas> smallAtlas, largeAtlas, numberAtlas;
   bool initialized;

   void loadFontAtlases(float pixelDensity);

public:
   TextRenderer();

   virtual ~TextRenderer();

   void init(float pixelDensity);

   void onPixelDensityChange(float pixelDensity);

   void render(int fbWidth, int fbHeight, float x, float y, const std::string &text, FontType fontType = FontType::Large, HAlign hAlign = HAlign::Left, VAlign vAlign = VAlign::Top);
};

#endif
