#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "MeshAssetManager.h"
#include "ShaderAssetManager.h"
#include "Types.h"

class Mesh;
class Shader;

class AssetManager {
protected:
   MeshAssetManager meshAssetManager;
   ShaderAssetManager shaderAssetManager;

public:
   AssetManager();

   virtual ~AssetManager();

   void reloadAssets();

   /**
    * Loads the shader with the given file name and type, using a cached version if possible
    */
   SPtr<Shader> loadShader(const std::string &fileName, const GLenum type);

   /**
    * Loads the mesh with the given file name, using a cached version if possible
    */
   SPtr<Mesh> loadMesh(const std::string &fileName);
};

#endif
