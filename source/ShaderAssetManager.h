#ifndef SHADER_ASSET_MANAGER_H
#define SHADER_ASSET_MANAGER_H

#include "GLIncludes.h"
#include "Types.h"

#include <string>
#include <unordered_map>

class Shader;
class ShaderProgram;

typedef std::unordered_map<std::string, SPtr<Shader>> ShaderMap;
typedef std::unordered_map<std::string, SPtr<ShaderProgram>> ShaderProgramMap;

class ShaderAssetManager {
protected:
   ShaderMap shaderMap;
   ShaderProgramMap shaderProgramMap;

public:
   ShaderAssetManager();

   virtual ~ShaderAssetManager();

   /**
    * Loads the shader with the given file name and type, using a cached version if possible
    */
   SPtr<Shader> loadShader(const std::string &fileName, const GLenum type);

   /**
    * Loads the shader program comprised of shaders with the given name (and their respective extensions), using a cached version if possible
    */
   SPtr<ShaderProgram> loadShaderProgram(const std::string &name);

   /**
    * Reloads all mapped shaders from source
    */
   void reloadShaders();
};

#endif
