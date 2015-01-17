#include "FancyAssert.h"
#include "TextureUnitManager.h"

TextureUnitManager::TextureUnitManager()
   : maxTextureUnits(0) {
}

TextureUnitManager::~TextureUnitManager() {
}

void TextureUnitManager::init() {
   glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
   ASSERT(maxTextureUnits > 0, "Max texture units is less than 1");

   reset();
}

void TextureUnitManager::reset() {
   textureUnits.assign(maxTextureUnits, true);
}

GLenum TextureUnitManager::get() {
   GLenum textureUnit = 0;
   bool assigned = false;

   for (int i = 0; i < textureUnits.size(); ++i) {
      if (textureUnits[i]) {
         textureUnits[i] = false;
         textureUnit = i;
         assigned = true;
         break;
      }
   }

   ASSERT(assigned, "No more available texture units");

   return textureUnit;
}

void TextureUnitManager::release(GLenum textureUnit) {
   ASSERT(textureUnit < textureUnits.size(), "Trying to release texture unit out of bounds");

   ASSERT(!textureUnits[textureUnit], "Trying to release already released texture unit");
   textureUnits[textureUnit] = false;
}
