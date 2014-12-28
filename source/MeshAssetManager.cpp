#include "IOUtils.h"
#include "LogHelper.h"
#include "Mesh.h"
#include "MeshAssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace {

/**
 * A cube
 */
const char* DEFAULT_MESH_SOURCE = "\nv -0.500000 -0.500000 0.500000\nv 0.500000 -0.500000 0.500000\nv -0.500000 0.500000 0.500000\nv 0.500000 0.500000 0.500000\nv -0.500000 0.500000 -0.500000\nv 0.500000 0.500000 -0.500000\nv -0.500000 -0.500000 -0.500000\nv 0.500000 -0.500000 -0.500000\n\nvt 0.000000 0.000000\nvt 1.000000 0.000000\nvt 0.000000 1.000000\nvt 1.000000 1.000000\n\nvn 0.000000 0.000000 1.000000\nvn 0.000000 1.000000 0.000000\nvn 0.000000 0.000000 -1.000000\nvn 0.000000 -1.000000 0.000000\nvn 1.000000 0.000000 0.000000\nvn -1.000000 0.000000 0.000000\n\ns 1\nf 1/1/1 2/2/1 3/3/1\nf 3/3/1 2/2/1 4/4/1\ns 2\nf 3/1/2 4/2/2 5/3/2\nf 5/3/2 4/2/2 6/4/2\ns 3\nf 5/4/3 6/3/3 7/2/3\nf 7/2/3 6/3/3 8/1/3\ns 4\nf 7/1/4 8/2/4 1/3/4\nf 1/3/4 8/2/4 2/4/4\ns 5\nf 2/1/5 8/2/5 4/3/5\nf 4/3/5 8/2/5 6/4/5\ns 6\nf 7/1/6 1/2/6 5/3/6\nf 5/3/6 1/2/6 3/4/6\n";

SPtr<Mesh> DEFAULT_MESH;

SPtr<Mesh> getDefaultMesh(Assimp::Importer &assimpImporter) {
   if (DEFAULT_MESH) {
      return DEFAULT_MESH;
   }

   unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_Triangulate;
   const aiScene *scene = assimpImporter.ReadFileFromMemory(DEFAULT_MESH_SOURCE, strlen(DEFAULT_MESH_SOURCE), flags);

   if (!scene) {
      LOG_FATAL("Unable to import default scene");
      return nullptr;
   }

   if (scene->mNumMeshes < 1) {
      LOG_FATAL("No meshes in default scene");
      return nullptr;
   }

   // TODO Support for multiple meshes
   DEFAULT_MESH = std::make_shared<Mesh>(scene->mMeshes[0]);
   return DEFAULT_MESH;
}

} // namespace

MeshAssetManager::MeshAssetManager()
   : assimpImporter(new Assimp::Importer) {
}

MeshAssetManager::~MeshAssetManager() {
}

SPtr<Mesh> MeshAssetManager::loadMesh(const std::string &fileName) {
   if (meshMap.count(fileName) > 0) {
      return meshMap[fileName];
   }

   if (!IOUtils::canReadData(fileName)) {
      LOG_WARNING("Unable to load mesh from file \"" << fileName << "\", reverting to default mesh");
      return getDefaultMesh(*assimpImporter);
   }

   unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_Triangulate;
   const aiScene *scene = assimpImporter->ReadFile(IOUtils::dataPath(fileName), flags);

   if (!scene) {
      LOG_WARNING("Unable to import scene \"" << fileName << "\", reverting to default mesh");
      return getDefaultMesh(*assimpImporter);
   }

   if (scene->mNumMeshes < 1) {
      LOG_WARNING("No meshes in scene \"" << fileName << "\", reverting to default mesh");
      return getDefaultMesh(*assimpImporter);
   }

   // TODO Support for multiple meshes
   SPtr<Mesh> mesh(std::make_shared<Mesh>(scene->mMeshes[0]));

   meshMap[fileName] = mesh;
   return mesh;
}
