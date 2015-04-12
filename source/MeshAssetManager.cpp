#include "IOUtils.h"
#include "FancyAssert.h"
#include "LogHelper.h"
#include "Mesh.h"
#include "MeshAssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace {

const char* CUBE_MESH_SOURCE = "v -0.500000 -0.500000 0.500000\nv 0.500000 -0.500000 0.500000\nv -0.500000 0.500000 0.500000\nv 0.500000 0.500000 0.500000\nv -0.500000 0.500000 -0.500000\nv 0.500000 0.500000 -0.500000\nv -0.500000 -0.500000 -0.500000\nv 0.500000 -0.500000 -0.500000\n\nvt 0.000000 0.000000\nvt 1.000000 0.000000\nvt 0.000000 1.000000\nvt 1.000000 1.000000\n\nvn 0.000000 0.000000 1.000000\nvn 0.000000 1.000000 0.000000\nvn 0.000000 0.000000 -1.000000\nvn 0.000000 -1.000000 0.000000\nvn 1.000000 0.000000 0.000000\nvn -1.000000 0.000000 0.000000\n\ns 1\nf 1/1/1 2/2/1 3/3/1\nf 3/3/1 2/2/1 4/4/1\ns 2\nf 3/1/2 4/2/2 5/3/2\nf 5/3/2 4/2/2 6/4/2\ns 3\nf 5/4/3 6/3/3 7/2/3\nf 7/2/3 6/3/3 8/1/3\ns 4\nf 7/1/4 8/2/4 1/3/4\nf 1/3/4 8/2/4 2/4/4\ns 5\nf 2/1/5 8/2/5 4/3/5\nf 4/3/5 8/2/5 6/4/5\ns 6\nf 7/1/6 1/2/6 5/3/6\nf 5/3/6 1/2/6 3/4/6\n";

const char* XY_PLANE_MESH_SOURCE = "v -1.000000 -1.000000 -0.000000\nv 1.000000 -1.000000 -0.000000\nv -1.000000 1.000000 0.000000\nv 1.000000 1.000000 0.000000\nvt 1.000000 0.000000\nvt 1.000000 1.000000\nvt 0.000000 1.000000\nvt 0.000000 0.000000\nvn 0.000000 -0.000000 1.000000\ns off\nf 2/1/1 4/2/1 3/3/1\nf 1/4/1 2/1/1 3/3/1\n";

SPtr<Mesh> getMeshFromMemory(Assimp::Importer &assimpImporter, const char *data) {
   unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_Triangulate;
   const aiScene *scene = assimpImporter.ReadFileFromMemory(data, strlen(data), flags);

   if (!scene) {
      LOG_FATAL("Unable to import scene from memory");
      return nullptr;
   }

   if (scene->mNumMeshes < 1) {
      LOG_FATAL("No meshes in scene loaded from memory");
      return nullptr;
   }

   return std::make_shared<Mesh>(scene->mMeshes[0]);
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
      return getMeshForShape(MeshShape::Cube);
   }

   unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_Triangulate;
   const aiScene *scene = assimpImporter->ReadFile(IOUtils::dataPath(fileName), flags);

   if (!scene) {
      LOG_WARNING("Unable to import scene \"" << fileName << "\", reverting to default mesh");
      return getMeshForShape(MeshShape::Cube);
   }

   if (scene->mNumMeshes < 1) {
      LOG_WARNING("No meshes in scene \"" << fileName << "\", reverting to default mesh");
      return getMeshForShape(MeshShape::Cube);
   }

   // TODO Support for multiple meshes
   SPtr<Mesh> mesh(std::make_shared<Mesh>(scene->mMeshes[0]));

   meshMap[fileName] = mesh;
   return mesh;
}

SPtr<Mesh> MeshAssetManager::getMeshForShape(MeshShape shape) {
   static SPtr<Mesh> cubeMesh = nullptr;
   static SPtr<Mesh> xyPlaneMesh = nullptr;

   switch (shape) {
      case MeshShape::Cube:
         if (!cubeMesh) {
            cubeMesh = getMeshFromMemory(*assimpImporter, CUBE_MESH_SOURCE);
         }
         return cubeMesh;
      case MeshShape::XYPlane:
         if (!xyPlaneMesh) {
            xyPlaneMesh = getMeshFromMemory(*assimpImporter, XY_PLANE_MESH_SOURCE);
         }
         return xyPlaneMesh;
      default:
         ASSERT(false, "Invalid mesh shape");
         return nullptr;
   }
}
