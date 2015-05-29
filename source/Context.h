#ifndef CONTEXT_H
#define CONTEXT_H

#include "Types.h"

#include <vector>

class AssetManager;
class AudioManager;
class InputHandler;
class Renderer;
class Scene;
class TextureUnitManager;
struct GLFWwindow;
struct InputValues;

namespace {

const int DEFAULT_SCORE_CAP = 5;

} // namespace

struct Player {
   int deviceNumber;
   int score;

   Player()
      : deviceNumber(-1), score(0) {
   }
};

struct GameSession {
   int scoreCap;
   bool currentLevelEnded;
   std::vector<Player> players;

   GameSession()
      : scoreCap(DEFAULT_SCORE_CAP), currentLevelEnded(false) {
   }
};

class Context {
private:
   static UPtr<Context> instance;

   Context(GLFWwindow* const window);

protected:
   GLFWwindow* const window;
   const UPtr<AssetManager> assetManager;
   const UPtr<AudioManager> audioManager;
   const UPtr<InputHandler> inputHandler;
   const UPtr<Renderer> renderer;
   const UPtr<TextureUnitManager> textureUnitManager;
   SPtr<Scene> scene;
   GameSession session;
   float runningTime;
   unsigned int activeShaderProgramID;
   bool menuAfterCurrentScene;
   bool quitAfterCurrentScene;

   void handleSpecialInputs(const InputValues &inputValues) const;

   void setScene(SPtr<Scene> scene);

   void updateSession();

   void checkForSceneChange();

public:
   static void load(GLFWwindow* const window);
   static Context& getInstance();

   virtual ~Context();

   void init();

   void quit() const;

   void quitAfterScene();

   void tick(const float dt);

   void onWindowFocusGained() const;

   AssetManager& getAssetManager() const;
   AudioManager& getAudioManager() const;
   InputHandler& getInputHandler() const;
   Renderer& getRenderer() const;
   Scene& getScene() const;
   TextureUnitManager& getTextureUnitManager() const;

   float getRunningTime() const {
      return runningTime;
   }

   unsigned int getActiveShaderProgramID() const {
      return activeShaderProgramID;
   }

   void setActiveShaderProgramID(unsigned int programID) {
      activeShaderProgramID = programID;
   }

   int getWindowWidth() const;

   int getWindowHeight() const;

   const GameSession& getGameSession() const {
      return session;
   }

   void changeScoreCap();
};

#endif
