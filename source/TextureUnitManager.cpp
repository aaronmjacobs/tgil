#include "FancyAssert.h"
#include "TextureUnitManager.h"

namespace {

const bool AVAILABLE = true;
const bool IN_USE = false;

} // namespace

TextureUnitManager::TextureUnitManager()
   : maxTextureUnits(0) {
}

TextureUnitManager::~TextureUnitManager() {
}

void TextureUnitManager::init() {
   glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
   ASSERT(maxTextureUnits > 0, "Max texture units is less than 1");

   reset();
}

void TextureUnitManager::reset() {
   textureUnits.assign(maxTextureUnits, AVAILABLE);
   textureUnits[getReservedShadowUnit()] = IN_USE; // Reserved for shadows
   textureUnits[getReservedCubeShadowUnit()] = IN_USE; // Reserved for cube shadows
}

GLenum TextureUnitManager::get() {
   GLenum textureUnit = 0;
   bool assigned = false;

   for (int i = 0; i < textureUnits.size(); ++i) {
      if (textureUnits[i] == AVAILABLE) {
         textureUnits[i] = IN_USE;
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
   textureUnits[textureUnit] = AVAILABLE;
}

GLenum TextureUnitManager::getReservedShadowUnit() const {
   ASSERT(textureUnits.size() > 0, "Not enough texture units to reserve for shadows");
   return textureUnits.size() - 1;
}

GLenum TextureUnitManager::getReservedCubeShadowUnit() const {
   ASSERT(textureUnits.size() > 1, "Not enough texture units to reserve for cube shadows");
   return textureUnits.size() - 2;
}
