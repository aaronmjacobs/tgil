#ifndef SHADER_ASSET_MANAGER_H
#define SHADER_ASSET_MANAGER_H

#include "GLIncludes.h"
#include "Types.h"

#include <map>
#include <string>

class Shader;

class ShaderAssetManager {
protected:
   std::map<std::string, SPtr<Shader>> shaderMap;

public:
   ShaderAssetManager();

   virtual ~ShaderAssetManager();

   /**
    * Loads the shader with the given file name and type, using a cached version if possible
    */
   SPtr<Shader> loadShader(const std::string &fileName, const GLenum type);
};

#endif
