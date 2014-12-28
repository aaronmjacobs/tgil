#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "GLIncludes.h"
#include "Types.h"

#include <map>

class Shader;

class AssetManager {
protected:
   std::map<std::string, SPtr<Shader>> shaderMap;

public:
   AssetManager();

   virtual ~AssetManager();

   /**
    * Loads the shader with the given file name and type, using a cached version if possible
    */
   SPtr<Shader> loadShader(const std::string &fileName, const GLenum type);
};

#endif
