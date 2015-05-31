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
   : window(window), assetManager(new AssetManager), audioManager(new AudioManager), inputHandler(new InputHandler(window)), renderer(new Renderer), textureUnitManager(new TextureUnitManager), state(ContextState::INIT), musicChangeInitiated(false), runningTime(0.0f), activeShaderProgramID(0), menuAfterCurrentScene(false), quitAfterCurrentScene(false) {
}

Context::~Context() {
}

void Context::init() {
   audioManager->init();
   textureUnitManager->init();

   // TODO Decouple player number from device number
   int numDevices = inputHandler->getNumDevices();
   for (int i = 0; i < numDevices; ++i) {
      Player player;
      player.deviceNumber = i;

      session.players.push_back(player);
   }
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

   RUN_DEBUG(
   static bool actionHeld = false;
   if (inputValues.action) {
      if (!actionHeld) {
         renderer->enableDebugRendering(!renderer->debugRenderingEnabled());
      }

      actionHeld = true;
   } else {
      actionHeld = false;
   }
   )
}

void Context::setScene(SPtr<Scene> scene) {
   session.currentLevelEnded = false;
   this->scene = scene;
}

void Context::updateSession() {
   if (!session.currentLevelEnded && scene->getGameState().hasWinner()) {
      int winner = scene->getGameState().getWinner();
      ASSERT(winner >= 0 && winner < session.players.size(), "Invalid winner index");

      session.currentLevelEnded = true;
      session.players[winner].score++;
   }
}

void Context::checkForSceneChange() {
   if (scene && scene->getTimeSinceEnd() < TIME_TO_NEXT_LEVEL) {
      return;
   }

   ContextState nextState = determineNextState();

   switch (nextState) {
      case ContextState::MENU:
         setScene(SceneLoader::loadMenuScene(*this));
         menuAfterCurrentScene = false;
         break;
      case ContextState::GAMEPLAY:
         setScene(SceneLoader::loadNextLevel(*this));
         break;
      case ContextState::WIN:
         setScene(SceneLoader::loadWinScene(*this));

         for (Player &player : session.players) {
            player.score = 0;
         }

         menuAfterCurrentScene = true;
         break;
      case ContextState::QUIT:
         quit();
         break;
      default:
         break;
   }

   state = nextState;
   musicChangeInitiated = false;
}

void Context::checkForMusicChange() {
   if ((scene && scene->getTimeSinceEnd() < (TIME_TO_NEXT_LEVEL - MUSIC_FADE_TIME)) || musicChangeInitiated) {
      return;
   }

   ContextState nextState = determineNextState();

   switch (nextState) {
      case ContextState::MENU:
         audioManager->play(SoundGroup::MENU_MUSIC);
         break;
      case ContextState::GAMEPLAY:
         if (state != ContextState::GAMEPLAY) {
            audioManager->play(SoundGroup::GAME_MUSIC);
         }
         break;
      case ContextState::WIN:
         audioManager->play(SoundGroup::WIN_MUSIC);
         break;
      case ContextState::QUIT:
         audioManager->play(SoundGroup::SILENCE);
         break;
      default:
         break;
   }

   musicChangeInitiated = true;
}

ContextState Context::determineNextState() {
   if (!scene) {
      return ContextState::MENU;
   }

   if (quitAfterCurrentScene) {
      return ContextState::QUIT;
   }

   if (menuAfterCurrentScene) {
      return ContextState::MENU;
   }

   for (const Player &player : session.players) {
      if (player.score >= session.scoreCap) {
         return ContextState::WIN;
      }
   }

   return ContextState::GAMEPLAY;
}

void Context::tick(const float dt) {
   checkForMusicChange();
   checkForSceneChange();

   updateSession();

   inputHandler->pollInput();

   handleSpecialInputs(inputHandler->getKeyMouseInputValues());

   scene->tick(dt);

   audioManager->update();

   runningTime += dt;
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

int Context::getWindowWidth() const {
   return renderer->getWindowWidth();
}

int Context::getWindowHeight() const {
   return renderer->getWindowHeight();
}

void Context::changeScoreCap() {
   switch (session.scoreCap) {
      case DEFAULT_SCORE_CAP:
         session.scoreCap = 10;
         break;
      case 10:
         session.scoreCap = 25;
         break;
      case 25:
         session.scoreCap = 50;
         break;
      case 50:
         session.scoreCap = 100;
         break;
      default:
         session.scoreCap = DEFAULT_SCORE_CAP;
         break;
   }
}
