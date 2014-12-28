#include "AssetManager.h"

AssetManager::AssetManager() {
}

AssetManager::~AssetManager() {
}

SPtr<Shader> AssetManager::loadShader(const std::string &fileName, const GLenum type) {
   return shaderAssetManager.loadShader(fileName, type);
}

SPtr<Mesh> AssetManager::loadMesh(const std::string &fileName) {
   return meshAssetManager.loadMesh(fileName);
}
