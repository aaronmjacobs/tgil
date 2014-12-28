#include "AssetManager.h"
#include "Context.h"

Context::Context()
   : assetManager(new AssetManager) {
}

Context::~Context() {
}

AssetManager& Context::getAssetManager() const {
   return *assetManager;
}
