#ifndef MESH_ASSET_MANAGER_H
#define MESH_ASSET_MANAGER_H

#include "Types.h"

#include <string>
#include <unordered_map>

class Mesh;

namespace Assimp {
class Importer;
} // namespace Assimp

typedef std::unordered_map<std::string, SPtr<Mesh>> MeshMap;

enum class MeshShape {
   Cube, XYPlane, OpenTopCube
};

class MeshAssetManager {
protected:
   const UPtr<Assimp::Importer> assimpImporter;
   MeshMap meshMap;

public:
   MeshAssetManager();

   virtual ~MeshAssetManager();

   /**
    * Loads the mesh with the given file name, using a cached version if possible
    */
   SPtr<Mesh> loadMesh(const std::string &fileName);

   /**
    * Gets a mesh with the given shape
    */
   SPtr<Mesh> getMeshForShape(MeshShape shape);
};

#endif
