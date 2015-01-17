#ifndef TEXTURE_UNIT_MANAGER_H
#define TEXTURE_UNIT_MANAGER_H

#include "GLIncludes.h"

#include <vector>

class TextureUnitManager {
protected:
   // All texture units (true = available, false = in use)
   std::vector<bool> textureUnits;

   // The maximum number of supported texture units
   GLint maxTextureUnits;

public:
   TextureUnitManager();

   virtual ~TextureUnitManager();

   void init();

   /**
    * Resets the texture unit manager, freeing all texture units
    */
   void reset();

   /**
    * Gets an available texture unit
    */
   GLenum get();

   /**
    * Releases the given texture unit, making it available
    */
   void release(GLenum textureUnit);
};

#endif
