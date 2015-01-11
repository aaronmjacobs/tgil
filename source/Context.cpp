#include "AssetManager.h"
#include "Context.h"
#include "FancyAssert.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "Scene.h"

// Static members

UPtr<Context> Context::instance;

void Context::load(GLFWwindow *const window) {
   ASSERT(!instance, "Trying to reload existing Context");
   if (!instance) {
      instance = std::move(UPtr<Context>(new Context(window)));
      instance->init();
   }
}

const Context& Context::getInstance() {
   ASSERT(instance, "Trying to get null Context instance");
   return *instance;
}

// Normal class members

Context::Context(GLFWwindow* const window)
   : assetManager(new AssetManager), inputHandler(new InputHandler(window)), renderer(new Renderer) {
}

Context::~Context() {
}

void Context::init() {
   scene = std::make_shared<Scene>();
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

Renderer& Context::getRenderer() const {
   return *renderer;
}

Scene& Context::getScene() const {
   return *scene;
}
