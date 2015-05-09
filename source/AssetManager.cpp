#include "AssetManager.h"

AssetManager::AssetManager() {
}

AssetManager::~AssetManager() {
}

void AssetManager::reloadAssets() {
   shaderAssetManager.reloadShaders();
}

SPtr<Shader> AssetManager::loadShader(const std::string &fileName, const GLenum type) {
   return shaderAssetManager.loadShader(fileName, type);
}

SPtr<ShaderProgram> AssetManager::loadShaderProgram(const std::string &fileName) {
   return shaderAssetManager.loadShaderProgram(fileName);
}

SPtr<Mesh> AssetManager::loadMesh(const std::string &fileName) {
   return meshAssetManager.loadMesh(fileName);
}

SPtr<Mesh> AssetManager::getMeshForShape(MeshShape shape) {
   return meshAssetManager.getMeshForShape(shape);
}

SPtr<Texture> AssetManager::loadTexture(const std::string &fileName, TextureWrap::Type wrap) {
   return textureAssetManager.loadTexture(fileName, wrap);
}

SPtr<Texture> AssetManager::loadCubemap(const std::string &path, const std::string &extension) {
   return textureAssetManager.loadCubemap(path, extension);
}
