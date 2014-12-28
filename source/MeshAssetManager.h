#ifndef MESH_ASSET_MANAGER_H
#define MESH_ASSET_MANAGER_H

#include "Types.h"

#include <map>
#include <string>

class Mesh;

namespace Assimp {
class Importer;
} // namespace Assimp

class MeshAssetManager {
protected:
   const UPtr<Assimp::Importer> assimpImporter;
   std::map<std::string, SPtr<Mesh>> meshMap;

public:
   MeshAssetManager();

   virtual ~MeshAssetManager();

   /**
    * Loads the mesh with the given file name, using a cached version if possible
    */
   SPtr<Mesh> loadMesh(const std::string &fileName);
};

#endif
