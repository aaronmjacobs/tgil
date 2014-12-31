#include "AssetManager.h"
#include "Context.h"
#include "Scene.h"

Context::Context()
   : assetManager(new AssetManager), scene(new Scene) {
}

Context::~Context() {
}

AssetManager& Context::getAssetManager() const {
   return *assetManager;
}

Scene& Context::getScene() const {
   return *scene;
}
