#include "AssetManager.h"
#include "Context.h"
#include "FancyAssert.h"
#include "InputHandler.h"
#include "Scene.h"

// Static members

UPtr<Context> Context::instance;

void Context::load(GLFWwindow *const window) {
   ASSERT(!instance, "Trying to reload existing Context");
   if (!instance) {
      instance = std::move(UPtr<Context>(new Context(window)));
   }
}

const Context& Context::getInstance() {
   ASSERT(instance, "Trying to get null Context instance");
   return *instance;
}

// Normal class members

Context::Context(GLFWwindow* const window)
   : assetManager(new AssetManager), inputHandler(new InputHandler(window)), scene(std::make_shared<Scene>()) {
}

Context::~Context() {
}

void Context::onWindowFocusGained() const {
   assetManager->reloadAssets();
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
