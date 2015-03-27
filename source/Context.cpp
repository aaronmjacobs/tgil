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

namespace {

const float TIME_TO_NEXT_LEVEL = 3.0f;

} // namespace

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
   : window(window), assetManager(new AssetManager), audioManager(new AudioManager), inputHandler(new InputHandler(window)), renderer(new Renderer), textureUnitManager(new TextureUnitManager), runningTime(0.0f), timeSinceWinner(-1.0f), activeShaderProgramID(0) {
}

Context::~Context() {
}

void Context::init() {
   audioManager->init();
   textureUnitManager->init();
   scene = SceneLoader::loadNextScene(*this);
}

void Context::handleSpecialInputs(const InputValues &inputValues) const {
   if (inputValues.quit) {
      // TODO Prevent cursor from being locked for all message boxes
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      boxer::Selection selection = boxer::show("Do you want to quit?", "Quit", boxer::Style::Question, boxer::Buttons::YesNo);
      if (selection == boxer::Selection::Yes) {
         glfwSetWindowShouldClose(window, true);
      } else {
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

void Context::checkForWinner() {
   if (timeSinceWinner < 0.0f && scene->getGameState().hasWinner()) {
      timeSinceWinner = runningTime;
   }

   if (timeSinceWinner >= 0.0f && runningTime - timeSinceWinner > TIME_TO_NEXT_LEVEL) {
      timeSinceWinner = -1.0f;
      scene = SceneLoader::loadNextScene(*this);
   }
}

void Context::tick(const float dt) {
   inputHandler->pollInput();

   handleSpecialInputs(inputHandler->getInputValues(0));

   scene->tick(dt);

   runningTime += dt;
   checkForWinner();
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
