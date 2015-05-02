#ifndef CONTEXT_H
#define CONTEXT_H

#include "Types.h"

class AssetManager;
class AudioManager;
class InputHandler;
class Renderer;
class Scene;
class TextureUnitManager;
struct GLFWwindow;
struct InputValues;

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
   float runningTime;
   unsigned int activeShaderProgramID;
   int windowWidth;
   int windowHeight;
   bool quitAfterCurrentScene;

   void handleSpecialInputs(const InputValues &inputValues) const;

   void checkForSceneChange();

public:
   static void load(GLFWwindow* const window);
   static Context& getInstance();

   virtual ~Context();

   void init();

   void quit() const;

   void quitAfterScene();

   void tick(const float dt);

   void onWindowSizeChanged(int width, int height);

   void onWindowFocusGained() const;

   AssetManager& getAssetManager() const;
   AudioManager& getAudioManager() const;
   InputHandler& getInputHandler() const;
   Renderer& getRenderer() const;
   Scene& getScene() const;
   TextureUnitManager& getTextureUnitManager() const;

   void setScene(SPtr<Scene> scene);

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
};

#endif
