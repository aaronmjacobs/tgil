#include "AssetManager.h"
#include "AudioManager.h"
#include "Context.h"
#include "FancyAssert.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "TextureUnitManager.h"

#include <boxer/boxer.h>

// Static members

UPtr<Context> Context::instance;

void Context::load(GLFWwindow *const window) {
   ASSERT(!instance, "Trying to reload existing Context");
   if (!instance) {
      instance = std::move(UPtr<Context>(new Context(window)));
      instance->init();
   }
}

Context& Context::getInstance() {
   ASSERT(instance, "Trying to get null Context instance");
   return *instance;
}

// Normal class members

Context::Context(GLFWwindow* const window)
   : window(window), assetManager(new AssetManager), audioManager(new AudioManager), inputHandler(new InputHandler(window)), renderer(new Renderer), textureUnitManager(new TextureUnitManager), runningTime(0.0f), activeShaderProgramID(0), windowWidth(0), windowHeight(0), quitAfterCurrentScene(false) {
}

Context::~Context() {
}

void Context::init() {
   glfwGetWindowSize(window, &windowWidth, &windowHeight);

   audioManager->init();
   textureUnitManager->init();
   scene = SceneLoader::loadNextScene(*this);
}

void Context::quit() const {
   glfwSetWindowShouldClose(window, true);
}

void Context::quitAfterScene() {
   quitAfterCurrentScene = true;
}

void Context::handleSpecialInputs(const InputValues &inputValues) const {
   if (inputValues.quit) {
      boxer::Selection selection = boxer::show("Do you want to quit?", "Quit", boxer::Style::Question, boxer::Buttons::YesNo);
      if (selection == boxer::Selection::Yes) {
         quit();
      }
   }

   static bool actionHeld = false;
   if (inputValues.action) {
      if (!actionHeld) {
         renderer->enableDebugRendering(!renderer->debugRenderingEnabled());
      }

      actionHeld = true;
   } else {
      actionHeld = false;
   }
}

void Context::checkForSceneChange() {
   if (scene->getTimeSinceEnd() > TIME_TO_NEXT_LEVEL) {
      if (quitAfterCurrentScene) {
         quit();
      } else {
         scene = SceneLoader::loadNextScene(*this);
      }
   }
}

void Context::tick(const float dt) {
   inputHandler->pollInput();

   handleSpecialInputs(inputHandler->getKeyMouseInputValues());

   scene->tick(dt);

   runningTime += dt;
   checkForSceneChange();
}

void Context::onWindowSizeChanged(int width, int height) {
   windowWidth = width;
   windowHeight = height;
}

void Context::onWindowFocusGained() const {
   assetManager->reloadAssets();
}

AssetManager& Context::getAssetManager() const {
   return *assetManager;
}

AudioManager& Context::getAudioManager() const {
   return *audioManager;
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

TextureUnitManager& Context::getTextureUnitManager() const {
   return *textureUnitManager;
}

void Context::setScene(SPtr<Scene> scene) {
   this->scene = scene;
}

int Context::getWindowWidth() const {
   return windowWidth;
}

int Context::getWindowHeight() const {
   return windowHeight;
}
