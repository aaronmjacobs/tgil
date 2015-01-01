#include "AssetManager.h"
#include "Context.h"
#include "InputHandler.h"
#include "Scene.h"

Context::Context(GLFWwindow* const window)
   : assetManager(new AssetManager), inputHandler(new InputHandler(window)), scene(new Scene) {
}

Context::~Context() {
}

AssetManager& Context::getAssetManager() const {
   return *assetManager;
}

InputHandler& Context::getInputHandler() const {
   return *inputHandler;
}

Scene& Context::getScene() const {
   return *scene;
}
