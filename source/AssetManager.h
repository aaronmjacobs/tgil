#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "MeshAssetManager.h"
#include "ShaderAssetManager.h"
#include "TextureAssetManager.h"
#include "Types.h"

class Mesh;
class Shader;

class AssetManager {
protected:
   MeshAssetManager meshAssetManager;
   ShaderAssetManager shaderAssetManager;
   TextureAssetManager textureAssetManager;

public:
   AssetManager();

   virtual ~AssetManager();

   void reloadAssets();

   /**
    * Loads the shader with the given file name and type, using a cached version if possible
    */
   SPtr<Shader> loadShader(const std::string &fileName, const GLenum type);

   /**
    * Loads the shader program comprised of shaders with the given name (and their respective extensions), using a cached version if possible
    */
   SPtr<ShaderProgram> loadShaderProgram(const std::string &fileName);

   /**
    * Loads the mesh with the given file name, using a cached version if possible
    */
   SPtr<Mesh> loadMesh(const std::string &fileName);

   /**
    * Loads the texture with the given file name, using a cached version if possible
    */
   GLuint loadTexture(const std::string &fileName, TextureWrap::Type wrap = TextureWrap::Edge);

   /**
    * Loads the cubemap at the given path, using a cached version if possible
    */
   GLuint loadCubemap(const std::string &path, const std::string &extension = "png");
};

#endif
